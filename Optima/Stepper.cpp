// Optima is a C++ library for solving linear and non-linear constrained optimization problems
//
// Copyright (C) 2014-2018 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "Stepper.hpp"

// Optima includes
#include <Optima/Constraints.hpp>
#include <Optima/Exception.hpp>
#include <Optima/IpSaddlePointMatrix.hpp>
#include <Optima/IpSaddlePointSolver.hpp>
#include <Optima/Matrix.hpp>
#include <Optima/Options.hpp>

namespace Optima {

struct Stepper::Impl
{
    /// The constraints of the optimization calculation
    Constraints constraints;

    /// The options for the optimization calculation
    Options options;

    /// The solution vector `s = [dx dy dz dw]`.
    Vector s;

    /// The right-hand side residual vector `r = [rx ry rz rw]`.
    Vector r;

    /// The matrices Z, W, L, U assuming the ordering x = [x(free) x(fixed)].
    Vector Z, W, L, U;

    /// The number of variables.
    Index n;

    /// The number of free and fixed variables.
    Index nx, nf;

    /// The number of equality constraints.
    Index m;

    /// The total number of variables (x, y, z, w).
    Index t;

    /// The interior-point saddle point solver.
    IpSaddlePointSolver solver;

    /// Construct a default Stepper::Impl instance.
    Impl()
    {}

    /// Construct a Stepper::Impl instance with given optimization problem constraints.
    Impl(const Constraints& constraints)
    : constraints(constraints)
    {
        // Initialize the members related to number of variables and constraints
        n  = constraints.numVariables();
        m  = constraints.numLinearEqualityConstraints();
        nf = constraints.variablesWithFixedValues().size();
        nx = n - nf;
        t  = 3*n + m;

        // Initialize Z and W with zeros (the dafault value for variables
        // with fixed values or no lower/upper bounds).
        Z = zeros(n);
        W = zeros(n);

        // Initialize L and U with ones (the dafault value for variables
        // with fixed values or no lower/upper bounds).
        L = ones(n);
        U = ones(n);

        // Initialize r and s with zeros.
        r = zeros(t);
        s = zeros(t);

        // Initialize the saddle point solver
        auto A = constraints.equalityConstraintMatrix();
        solver.initialize(A);
    }

    /// Decompose the interior-point saddle point matrix for diagonal Hessian matrices.
    auto decompose(const StepperProblem& problem) -> void
    {
        // Auxiliary references
        const auto x = problem.x;
        const auto z = problem.z;
        const auto w = problem.w;
        const auto H = problem.H;
        const auto xlower = problem.xlower;
        const auto xupper = problem.xupper;

        // The indices of the variables with lower and upper bounds and fixed values
        IndicesConstRef ilower = constraints.variablesWithLowerBounds();
        IndicesConstRef iupper = constraints.variablesWithUpperBounds();
        IndicesConstRef ifixed = constraints.variablesWithFixedValues();

        // Update Z and L for the variables with lower bounds
        Z(ilower) = z(ilower);
		L(ilower) = x(ilower) - xlower;

        // Update W and U for the variables with upper bounds
        W(iupper) = w(iupper);
        U(iupper) = x(iupper) - xupper;

        // Ensure entries in L are positive in case x[i] == lowerbound[i]
		for(Index i : ilower) L[i] = L[i] > 0.0 ? L[i] : options.mu;

        // Ensure entries in U are negative in case x[i] == upperbound[i]
		for(Index i : iupper) U[i] = U[i] < 0.0 ? U[i] : -options.mu;

        // The matrix A in the interior-point saddle point matrix
        auto A = constraints.equalityConstraintMatrix();

        // Define the interior-point saddle point matrix
        IpSaddlePointMatrix spm(H, A, Z, W, L, U, ifixed);

        // Decompose the interior-point saddle point matrix
        solver.decompose(spm);
    }

    /// Solve the interior-point saddle point matrix.
    auto solve(const StepperProblem& problem) -> void
    {
        // Auxiliary references
        auto x = problem.x;
        auto y = problem.y;
        auto z = problem.z;
        auto w = problem.w;
        auto g = problem.g;

        // Alias to constraints variables
        auto A = constraints.equalityConstraintMatrix();

        // The indices of the variables with lower and upper bounds and fixed values
        IndicesConstRef ilower = constraints.variablesWithLowerBounds();
        IndicesConstRef iupper = constraints.variablesWithUpperBounds();
        IndicesConstRef ifixed = constraints.variablesWithFixedValues();

        // Views to the sub-vectors in r = [a b c d]
        auto a = r.head(n);
        auto b = r.segment(n, m);
        auto c = r.segment(n + m, n);
        auto d = r.tail(n);

        // Calculate the optimality residual vector a
        a.noalias() = -(g + tr(A) * y - z - w);

        // Set a to zero for fixed variables
        a(ifixed).fill(0.0);

        // Calculate the feasibility residual vector b
        b.noalias() = -(A*x - problem.b);

        // Calculate the centrality residual vectors c and d
        for(Index i : ilower) c[i] = options.mu - L[i] * z[i]; // TODO Check if mu is still needed. Maybe this algorithm no longer needs perturbation.
        for(Index i : iupper) d[i] = options.mu - U[i] * w[i];

//        c.fill(0.0); // TODO For example, there is no mu here and this seems to work
//        d.fill(0.0);

        // The right-hand side vector of the interior-point saddle point problem
        IpSaddlePointVector rhs(r, n, m);

        // The solution vector of the interior-point saddle point problem
        IpSaddlePointSolution sol(s, n, m);

        // Solve the saddle point problem
        solver.solve(rhs, sol);
    }

    /// Return the calculated Newton step vector.
    auto step() const -> IpSaddlePointVector
    {
        return IpSaddlePointVector(s, n, m);
    }

    /// Return the calculated residual vector for the current optimum state.
    auto residual() const -> IpSaddlePointVector
    {
        return IpSaddlePointVector(r, n, m);
    }

    /// Return the assembled interior-point saddle point matrix.
    auto matrix(const StepperProblem& problem) -> IpSaddlePointMatrix
    {
        // Auxiliary references
        auto H = problem.H;

        // The indices of the variables with fixed values
        IndicesConstRef ifixed = constraints.variablesWithFixedValues();

        // The matrix A in the interior-point saddle point matrix
        auto A = constraints.equalityConstraintMatrix();

        // Define the interior-point saddle point matrix
        return IpSaddlePointMatrix(H, A, Z, W, L, U, ifixed);
    }
};

Stepper::Stepper()
: pimpl(new Impl())
{}

Stepper::Stepper(const Constraints& constraints)
: pimpl(new Impl(constraints))
{}

Stepper::Stepper(const Stepper& other)
: pimpl(new Impl(*other.pimpl))
{}

Stepper::~Stepper()
{}

auto Stepper::operator=(Stepper other) -> Stepper&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto Stepper::setOptions(const Options& options) -> void
{
    pimpl->options = options;
    pimpl->solver.setOptions(options.kkt);
}

auto Stepper::decompose(const StepperProblem& problem) -> void
{
    return pimpl->decompose(problem);
}

auto Stepper::solve(const StepperProblem& problem) -> void
{
    return pimpl->solve(problem);
}

auto Stepper::matrix(const StepperProblem& problem) -> IpSaddlePointMatrix
{
    return pimpl->matrix(problem);
}

auto Stepper::step() const -> IpSaddlePointVector
{
    return pimpl->step();
}

auto Stepper::residual() const -> IpSaddlePointVector
{
    return pimpl->residual();
}

} // namespace Optima
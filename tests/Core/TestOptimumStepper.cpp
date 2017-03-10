// Optima is a C++ library for numerical sol of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2017 Allan Leal
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

#include <doctest/doctest.hpp>

// Eigen includes
#include <Eigenx/LU.hpp>
using namespace Eigen;

// Optima includes
#include <Optima/Core/OptimumOptions.hpp>
#include <Optima/Core/OptimumParams.hpp>
#include <Optima/Core/OptimumState.hpp>
#include <Optima/Core/OptimumStepper.hpp>
#include <Optima/Core/OptimumStructure.hpp>
#include <Optima/Math/Matrix.hpp>
using namespace Optima;

#define PRINT_STATE                                                         \
{                                                                           \
    std::cout << "M = \n" << M << std::endl;                                \
    std::cout << "r    = " << tr(r) << std::endl;                           \
    std::cout << "s    = " << tr(M.fullPivLu().solve(r)) << std::endl;      \
    std::cout << "step = " << tr(step) << std::endl;                        \
    std::cout << "res  = " << tr(res) << std::endl;                         \
}                                                                           \

TEST_CASE("Testing OptimumStepper")
{
    Index n = 60;
    Index m = 10;
    Index t = 2*n + m;

    MatrixXd A = random(m, n);
    MatrixXd H = random(n, n);
    VectorXd g = random(n);
    VectorXd a = random(m);
    VectorXd x = abs(random(n));
    VectorXd y = random(m);
    VectorXd z = random(n);

    OptimumOptions options;

    auto assemble_matrix = [&]()
    {
        MatrixXd M = zeros(t, t);
        M.topLeftCorner(n, n) = H;
        M.topRows(n).middleCols(n, m) = -tr(A);
        M.topRightCorner(n, n).diagonal().fill(-1.0);
        M.middleRows(n, m).leftCols(n) = A;
        M.bottomLeftCorner(n, n).diagonal() = z;
        M.bottomRightCorner(n, n).diagonal() = x;
        return M;
    };

    auto assemble_vector = [&]()
    {
        VectorXd r = zeros(t);
        r.head(n) = -(g - tr(A)*y - z);
        r.segment(n, m) = -(A*x - a);
        r.tail(n) = -(x % z - options.mu);
        return r;
    };

    auto compute_step = [&]()
    {
        OptimumStructure structure;
        structure.n = n;
        structure.A = A;

        OptimumParams params;
        params.a = a;
        params.xlower = zeros(n);

        OptimumState state;
        state.x = x;
        state.y = y;
        state.z = z;

        ObjectiveState f;
        f.grad = g;
        f.hessian = H;

        OptimumStepper stepper;
        stepper.initialize(structure);
        stepper.decompose(params, state, f);
        stepper.solve(params, state, f);

        return VectorXd(stepper.step());
    };

    SUBCASE("when all variables are stable")
    {
        MatrixXd M = assemble_matrix();
        MatrixXd r = assemble_vector();
        VectorXd step = compute_step();
        VectorXd res = M*step - r;

        PRINT_STATE;

        CHECK(norm(res)/norm(r) == approx(0.0));
    }

    SUBCASE("when the first `m` variables are unstable")
    {
        z.head(m).fill(1.0);
        x.head(m).fill(1e-16);

        MatrixXd M = assemble_matrix();
        MatrixXd r = assemble_vector();
        VectorXd step = compute_step();
        VectorXd res = M*step - r;

        PRINT_STATE;

        res.head(m).fill(0.0);
        CHECK(norm(res)/norm(r) == approx(0.0));
    }

    SUBCASE("when the first `m` variables are unstable and Huu has large diagonal entries")
    {
        z.head(m).fill(1.0);
        x.head(m).fill(1e-16);
        H.topLeftCorner(m, m) = 1e16*identity(m, m);

        MatrixXd M = assemble_matrix();
        MatrixXd r = assemble_vector();
        VectorXd step = compute_step();
        VectorXd res = M*step - r;

        PRINT_STATE;

        res.head(m).fill(0.0);
        CHECK(norm(res)/norm(r) == approx(0.0));
    }

}

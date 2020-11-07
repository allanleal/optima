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

#include "NewtonStep.hpp"

// Optima includes
#include <Optima/Exception.hpp>
#include <Optima/LinearSolver.hpp>

namespace Optima {

struct NewtonStep::Impl
{
    MasterDims dims;           ///< The dimensions of the master variables.
    LinearSolver linearsolver; ///< The linear solver for the master matrix equations.
    MasterVector du;           ///< The Newton step for master variables u = (x, p, w).
    Vector xlower;             ///< The lower bounds for variables *x*.
    Vector xupper;             ///< The upper bounds for variables *x*.

    Impl(const MasterDims& dims)
    : dims(dims), linearsolver(dims), du(dims.nx, dims.np, dims.nw)
    {
    }

    auto initialize(NewtonStepInitializeArgs args) -> void
    {
        xlower = args.xlower;
        xupper = args.xupper;
        linearsolver.setOptions(args.options.linearsolver);
        sanitycheck();
    }

    auto apply(const ResidualFunction& F, MasterVectorView uo, MasterVectorRef u) -> void
    {
        sanitycheck();
        const auto Mc = F.canonicalJacobianMatrix();
        const auto ac = F.canonicalResidualVector();
        linearsolver.decompose(Mc);
        linearsolver.solve(Mc, ac, du);
        u.x .noalias() = uo.x + du.x;
        u.p .noalias() = uo.p + du.p;
        u.w .noalias() = uo.w + du.w;
        u.x.noalias() = min(max(u.x, xlower), xupper);
    }

    auto sanitycheck() const -> void
    {
        assert(xlower.size() == dims.nx);
        assert(xupper.size() == dims.nx);
    }
};

NewtonStep::NewtonStep(const MasterDims& dims)
: pimpl(new Impl(dims))
{}

NewtonStep::NewtonStep(const NewtonStep& other)
: pimpl(new Impl(*other.pimpl))
{}

NewtonStep::~NewtonStep()
{}

auto NewtonStep::operator=(NewtonStep other) -> NewtonStep&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto NewtonStep::initialize(NewtonStepInitializeArgs args) -> void
{
    pimpl->initialize(args);
}

auto NewtonStep::apply(const ResidualFunction& F, MasterVectorView uo, MasterVectorRef u) -> void
{
    pimpl->apply(F, uo, u);
}

} // namespace Optima
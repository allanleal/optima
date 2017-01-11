// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
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

#pragma once

// Optima includes
#include <Optima/Math/Matrix.hpp>
#include <Optima/Core/Hessian.hpp>
#include <Optima/Core/OptimumProblem.hpp>

namespace Optima {

/// A type that describes the state of an optimum solution
struct OptimumState
{
    /// The primal solution of the optimization problem
    Vector x;

    /// The dual solution of the optimization problem with respect to the equality constraints
    Vector y;

    /// The dual solution of the optimization problem with respect to the lower bound constraints
    Vector z;

    /// The dual solution of the optimization problem with respect to the upper bound constraints
    Vector w;

    /// The evaluation of the objective function at `x`
    ObjectiveState f;
};

} // namespace Optima

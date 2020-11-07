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

#pragma once

// C++ includes
#include <memory>

// Optima includes
#include <Optima/MasterVector.hpp>
#include <Optima/NewtonStepOptions.hpp>
#include <Optima/ResidualFunction.hpp>

namespace Optima {

/// The arguments for method @ref NewtonStep::initialize.
struct NewtonStepInitializeArgs
{
    /// The lower bounds for variables *x*.
    const VectorConstRef xlower;

    /// The upper bounds for variables *x*.
    const VectorConstRef xupper;

    /// The options for the Newton step calculation.
    const NewtonStepOptions options;
};

/// Used to update the variables in an optimization problem using Newton steps.
class NewtonStep
{
private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;

public:
    /// Construct a NewtonStep object.
    NewtonStep(const MasterDims& dims);

    /// Construct a copy of a NewtonStep object.
    NewtonStep(const NewtonStep& other);

    /// Destroy this NewtonStep object.
    virtual ~NewtonStep();

    /// Assign a NewtonStep object to this.
    auto operator=(NewtonStep other) -> NewtonStep&;

    /// Initialize this NewtonStep object.
    auto initialize(NewtonStepInitializeArgs args) -> void;

    /// Apply Newton step to compute the next state of master variables.
    auto apply(const ResidualFunction& F, MasterVectorView uo, MasterVectorRef u) -> void;
};

} // namespace Optima
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
#include <Optima/MasterDims.hpp>
#include <Optima/CanonicalMatrix.hpp>
#include <Optima/CanonicalVector.hpp>

namespace Optima {

/// Used to solve linear problems in their canonical form.
class LinearSolverRangespace
{
public:
    /// Construct a LinearSolverRangespace instance.
    LinearSolverRangespace(const MasterDims& dims);

    /// Construct a copy of a LinearSolverRangespace instance.
    LinearSolverRangespace(const LinearSolverRangespace& other);

    /// Destroy this LinearSolverRangespace instance.
    virtual ~LinearSolverRangespace();

    /// Assign a LinearSolverRangespace instance to this.
    auto operator=(LinearSolverRangespace other) -> LinearSolverRangespace&;

    /// Decompose the canonical matrix.
    auto decompose(CanonicalMatrixView M) -> void;

    /// Solve the linear problem in its canonical form.
    /// Using this method presumes method @ref decompose has already been
    /// called. This will allow you to reuse the decomposition of the master
    /// matrix for multiple solve computations if needed.
    /// @param M The canonical matrix in the canonical linear problem.
    /// @param a The right-hand side canonical vector in the canonical linear problem.
    /// @param[out] u The solution  vector in the canonical linear problem.
    auto solve(CanonicalMatrixView M, CanonicalVectorView a, CanonicalVectorRef u) -> void;

private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;
};

} // namespace Optima

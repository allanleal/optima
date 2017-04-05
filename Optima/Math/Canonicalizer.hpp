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

// C++ includes
#include <memory>

// Optima includes
#include <Optima/Common/Index.hpp>
#include <Optima/Math/Matrix.hpp>

namespace Optima {

/// Used to describe a matrix \eq{A} in canonical form.
/// The canonical form of a matrix \eq{A} is represented as:
/// \eqq{C = RAQ = \begin{bmatrix}I & S\end{bmatrix},}
/// where \eq{Q} is a permutation matrix, and \eq{R} is the *canonicalizer matrix* of \eq{A}.
class Canonicalizer
{
public:
    /// Construct a default Canonicalizer instance.
    Canonicalizer();

    /// Construct a Canonicalizer instance with given matrix.
    Canonicalizer(MatrixXdConstRef A);

    /// Construct a copy of a Canonicalizer instance.
    Canonicalizer(const Canonicalizer& other);

    /// Destroy this Canonicalizer instance.
    virtual ~Canonicalizer();

    /// Assign a Canonicalizer instance to this.
    auto operator=(Canonicalizer other) -> Canonicalizer&;

    /// Return the number of variables.
    auto numVariables() const -> Index;

    /// Return the number of basic variables.
    auto numBasicVariables() const -> Index;

    /// Return the number of non-basic variables.
    auto numNonBasicVariables() const -> Index;

    /// Return the matrix \eq{S} of the canonicalization.
    auto S() const -> const MatrixXd&;

    /// Return the canonicalizer matrix \eq{R}.
    auto R() const -> const MatrixXd&;

    /// Return the permutation matrix \eq{Q} of the canonicalization.
    auto Q() const -> const PermutationMatrix&;

    /// Return the canonicalized matrix \eq{C = RAQ = [I\quad S]}`.
    auto C() const -> MatrixXd;

    /// Return the indices of the variables describing its ordering after canonicalization.
    auto ordering() const -> VectorXiConstRef;

    /// Return the indices of the linearly independent rows of the original matrix.
    auto ili() const -> VectorXi;

    /// Return the indices of the basic components.
    auto ibasic() const -> VectorXiConstRef;

    /// Return the indices of the non-basic components.
    auto inonbasic() const -> VectorXiConstRef;

	/// Compute the canonical matrix of the given matrix.
	auto compute(MatrixXdConstRef A) -> void;

	/// Rationalize the entries in the canonical form.
	/// This method should be used if the entries in matrix \eq{A} are rational numbers and
	/// round-off errors introduced by the canonicalization should be eliminated as much as possible.
	/// This method will replace all entries in matrices \eq{R} and \eq{S} by the nearest rational
	/// number. To to this, an estimate for the maximum denominator among all entries in \eq{A}
	/// is needed. For example, one might know in advance that all entries in \eq{A} are rationals
	/// with denominators certainly not greater than 1000.
	/// @param maxdenominator The estimate for the maximum denominator.
	auto rationalize(Index maxdenominator) -> void;

	/// Swap a basic variable by a non-basic variable.
	/// @param ibasic The index of the basic variable between 0 and \eq{n_\mathrm{b}}`.
	/// @param inonbasic The index of the non-basic variable between 0 and \eq{n_\mathrm{n}}`.
	auto swapBasicVariable(Index ibasic, Index inonbasic) -> void;

	/// Update the canonical form with given priority weights for the variables.
	/// This method will update the canonical form by taking into account the given priority
	/// weights of the variables when selecting the basic variables.
	/// The basic and non-basic variables will be sorted in descend order with respect to their
	/// priority weights.
	/// By choosing non-positive weights for some variables, and positive for all others, the
	/// variables with non-positive weights can be prevented from becoming basic variables.
	/// However, there is the possibility of a *degenerate case* in which one or more variables
	/// with non-positive weights need to be basic variables. This happens when all variables with
	/// non-zero coefficient in a row of matrix \eq{A} have non-positive weights.
    /// @param weights The priority weights of the variables.
	auto update(VectorXdConstRef weights) -> void;

	/// Update the order of the variables.
	auto update(VectorXiConstRef ordering) -> void;

private:
	struct Impl;

	std::unique_ptr<Impl> pimpl;
};

} // namespace Optima

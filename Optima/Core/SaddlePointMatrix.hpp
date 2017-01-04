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
#include <Optima/Common/Index.hpp>
#include <Optima/Common/Optional.hpp>
#include <Optima/Math/Matrix.hpp>

namespace Optima {

/// Used to represent the coefficient matrix in a saddle point problem.
/// A saddle point matrix is defined as a matrix with the following structure:
/// \f[
/// \begin{bmatrix}H & A^{T}\\ A & 0 \end{bmatrix}\begin{bmatrix}x\\ y \end{bmatrix}=\begin{bmatrix}a\\ b \end{bmatrix},
/// \f]
/// where \eq{H} is the *Hessian matrix* of an objective function and \eq{A} is
/// the *Jacobian matrix* of a constraint function. If the saddle point problem has fixed variables,
/// then the saddle point matrix has the following representation:
/// \f[
/// \begin{bmatrix}H_{\mathrm{x}} & 0 & A_{\mathrm{x}}^{T}\\ 0 & I_{\mathrm{f}} & 0\\ A_{\mathrm{x}} & A_{\mathrm{f}} & 0 \end{bmatrix}\begin{bmatrix}x_{\mathrm{x}}\\ x_{\mathrm{f}}\\ y \end{bmatrix}=\begin{bmatrix}a_{\mathrm{x}}\\ a_{\mathrm{f}}\\ b \end{bmatrix},
/// \f]
/// where the subscripts \eq{\mathrm{x}} and \eq{\mathrm{f}} correspond to free and fixed variables, respectively.
class SaddlePointMatrix
{
public:
    /// Construct a SaddlePointMatrix instance with given Hessian and Jacobian matrices.
    SaddlePointMatrix(ConstMatrixRef H, ConstMatrixRef A);

    /// Construct a SaddlePointMatrix instance with given Hessian and Jacobian matrices, and indices of fixed variables.
    SaddlePointMatrix(ConstMatrixRef H, ConstMatrixRef A, const Indices& fixed);

    /// Return the Hessian matrix *H*.
    auto hessian() const -> ConstMatrixRef;

    /// Return the Jacobian matrix *A*.
    auto jacobian() const -> ConstMatrixRef;

    /// Return the indices of the fixed variables.
    auto fixed() const -> const Indices&;

    /// Convert this SaddlePointMatrix instance into a Matrix instance.
    auto matrix() const -> MatrixXd;

private:
    /// The Hessian matrix \eq{H} in the saddle point matrix.
    ConstMatrixRef m_H;

    /// The Jacobian matrix \eq{A} in the saddle point matrix.
    ConstMatrixRef m_A;

    /// The indices of the fixed variables.
    Optional<Indices> m_fixed;
};

/// A type used to describe a saddle point right-hand side vector.
struct SaddlePointVector
{
    /// The saddle-point vector `x`.
    VectorXd x;

    /// The saddle-point vector `y`.
    VectorXd y;

    /// Convert this SaddlePointVector instance into a Vector instance.
    auto vector() const -> VectorXd;
};

/// Return the multiplication of a saddle point matrix and a saddle point vector.
auto operator*(const SaddlePointMatrix& A, const SaddlePointVector& x) -> SaddlePointVector;

} // namespace Optima


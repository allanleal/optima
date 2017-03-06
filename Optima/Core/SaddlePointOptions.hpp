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

namespace Optima {

/// Used to describe the possible methods for solving saddle point problems.
enum class SaddlePointMethod
{
    /// This method uses partial-pivoting LU decomposition to fully solve the saddle point problem.
    /// This method solves the saddle point problem by applying a partial-pivoting
    /// LU decomposition to the saddle point matrix of dimension \eq{(n+m)\times(n+m)}.
    /// This method is in general accurate enough, but less accurate than its
    /// full pivoting counterpart. In general, it is also faster than the other methods
    /// for problems with small dimensions and when \eq{n} is not too larger than \eq{m}.
    /// @note This method takes no advantage of the particular structure of the saddle point matrix.
    PartialPivLU,

    /// This method uses full-pivoting LU decomposition to fully solve the saddle point problem.
    /// This method solves the saddle point problems by applying a full-pivoting LU decomposition
    /// to the saddle point matrix. It is in general accurate, but also more computationally expensive.
    /// @note This method takes no advantage of the particular structure of the saddle point matrix.
    FullPivLU,

    /// This method reduces the dimension of the saddle point problem from \eq{n+m} to \eq{n-m}.
    /// This method reduces the saddle point problem of dimension \eq{n+m} to an equivalent one of
    /// dimension \eq{n-m}, where \eq{n \times n} is the dimension of the Hessian matrix \eq{H} and
    /// \eq{m \times n} is the dimension of the Jacobian matrix \eq{A}.
    /// This method is suitable when matrix \eq{H} in the saddle point problem is dense and \eq{A}
    /// has relatively many rows to sufficiently decrease the size of the linear system.
    Nullspace,

    /// This method reduces the dimension of the saddle point problem from \eq{n+m} to \eq{m}.
    /// This method reduces the saddle point problem of dimension \eq{n+m} to an equivalent one of
    /// dimension \eq{m}, where these dimensions are related to the dimensions of the Hessian matrix
    /// \eq{H}, \eq{n \times n}, and Jacobian matrix \eq{A}, \eq{m \times n}.
    /// @warning This method should only be used when the Hessian matrix is a diagonal matrix.
    RangespaceDiagonal,
};

/// Used to specify the options for the solution of saddle point problems.
/// @see SaddlePointSolver
struct SaddlePointOptions
{
    /// The method for solving the saddle point problems.
    SaddlePointMethod method = SaddlePointMethod::PartialPivLU;

    /// The option to rationalize the entries in the canonical form.
    /// This option should be turned on if accuracy of the calculations is  sensitive to round-off
    /// errors and the entries in the coefficient matrix \eq{A} of the saddle point problem are
    /// rational numbers. This method will clean round-off errors after canonicalization operations
    /// by computing the rational number of each entry in the canonical form.
    bool rationalize = false;
};


} // namespace Optima

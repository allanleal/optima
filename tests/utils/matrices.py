# Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
#
# Copyright (C) 2014-2018 Allan Leal
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from optima import *
from numpy import *

from utils.data import formula_matrix


def pascal_matrix(m, n):
    """Return a Pascal matrix with given dimensions.

    Arguments:
        m {int} -- The number of rows in the matrix
        n {int} -- The number of columns in the matrix
    """
    A = ones((m, n))
    for i in range(1, m):
        for j in range(1, n):
            A[i, j] = A[i, j - 1] + A[i - 1, j]
    return A


def matrix_with_linearly_independent_rows_only(m, n, ifixed=[]):
    """Return a matrix with linearly independent rows only based on Pascal triangle.

    Arguments:
        m {int} -- The number of rows in the matrix
        n {int} -- The number of columns in the matrix

    Keyword Arguments:
        ifixed {int list} -- The indices of the fixed variables (default: {[]})

    Returns:
        [array] -- The matrix with asked structure.
    """
    assert m <= n
    q,r = linalg.qr(pascal_matrix(n, n))
    return q[:m, :] @ q


def assemble_matrix_Ax(m, n, nbu, nl, ju):
    """Return a matrix Ax for testing purposes.

    Arguments:
        m {int} -- The number of rows in matrix A
        n {int} -- The number of columns in matrix A
        nbu {int} -- The number of basic unstable variables
        nl {int} -- The number of linearly dependent rows in A
        ju {int list} -- The indices of the unstable variables

    Returns:
        [array] -- The matrix A with asked structure and features.
    """
    assert m <= n
    q,r = linalg.qr(pascal_matrix(n, n))
    Ax = q[:m, :] @ q
    if len(ju) != 0:
        js = list(set(range(n)) - set(ju))  # indices of stable variables
        for k in range(nbu):
            Ax[int(m / (2*(k + 1))), js] = 0.0  # all zeros, except columns corresponding to unstable/fixed variables
    for k in range(nl):
        Ax[m - k - 1, :] = Ax[k, :]  # create linear dependency: last rows become first rows
    return Ax


def matrix_with_one_linearly_dependent_row(m, n, ifixed=[]):
    """Return a matrix with one linearly dependent row.

    Arguments:
        m {int} -- The number of rows in the matrix
        n {int} -- The number of columns in the matrix

    Keyword Arguments:
        ifixed {int list} -- The indices of the fixed variables (default: {[]})

    Returns:
        [array] -- The matrix with asked structure.
    """
    A = matrix_with_linearly_independent_rows_only(m, n, ifixed)
    A[2, :] = 2*A[0, :] + A[1, :]
    return A


def matrix_with_two_linearly_dependent_rows(m, n, ifixed=[]):
    """Return a matrix with two linearly dependent rows.

    Arguments:
        m {int} -- The number of rows in the matrix
        n {int} -- The number of columns in the matrix

    Keyword Arguments:
        ifixed {int list} -- The indices of the fixed variables (default: {[]})

    Returns:
        [array] -- The matrix with asked structure.
    """
    A = matrix_with_linearly_independent_rows_only(m, n, ifixed)
    A[1, :] = 2*A[0, :] + A[-1, :]
    A[2, :] = A[0, :]
    return A


def matrix_with_one_basic_fixed_variable(m, n, ifixed=[]):
    A = matrix_with_linearly_independent_rows_only(m, n, ifixed)
    if len(ifixed) != 0:
        ifree = list(set(range(n)) - set(ifixed))  # indices of free variables
        A[int(m/2), ifree] = 0.0  # middle row is all zeros, except columns corresponding to fixed variables
    return A


def matrix_with_two_basic_fixed_variables(m, n, ifixed=[]):
    A = matrix_with_linearly_independent_rows_only(m, n, ifixed)
    if len(ifixed) != 0:
        ifree = list(set(range(n)) - set(ifixed))  # indices of free variables
        A[int(m/2), ifree] = 0.0  # middle row is all zeros, except columns corresponding to fixed variables
        A[int(m/4), ifree] = 0.0  # quarter row is all zeros, except columns corresponding to fixed variables
    return A


def matrix_with_one_zero_column(m, n, ifixed=[]):
    A = matrix_with_linearly_independent_rows_only(m, n, ifixed)
    A[:, 0] = 0.0  # first column of the matrix is zero (i.e. when first variable does not participate in any linear constraints)
    return A


def matrix_with_two_zero_columns(m, n, ifixed=[]):
    A = matrix_with_linearly_independent_rows_only(m, n, ifixed)
    A[:, 0] = 0.0  # first column of the matrix is zero (i.e. when first variable does not participate in any linear constraints)
    A[:, 1] = 0.0  # second column of the matrix is zero (i.e. when second variable does not participate in any linear constraints)
    return A


def matrix_non_singular(n):
    u,s,vh = linalg.svd(pascal_matrix(n, n))
    s = linspace(1.0, n, num=n)
    q = u @ diag(s) @ vh
    return q


# The functions that create matrices with different structures
testing_matrices_A = [
    # matrix_with_linearly_independent_rows_only,
    # matrix_with_one_linearly_dependent_row,
    matrix_with_two_linearly_dependent_rows,
    # matrix_with_one_basic_fixed_variable,
    # matrix_with_two_basic_fixed_variables,
    # matrix_with_one_zero_column,
    # matrix_with_two_zero_columns
]

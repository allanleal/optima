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
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from optima import *
from numpy import *
from numpy.linalg import norm
from numpy.testing import assert_allclose
from pytest import approx, mark
from itertools import product

from utils.matrices import matrix_non_singular

# Tested number of variables in x
tested_n = [20, 40, 60]
# tested_n = [5, 10]

# Tested rank deficiency of matrix A
tested_rank_deficiency = [0, 1, 5, 10, 15]
# tested_rank_deficiency = [4]

# Combination of all tested cases
testdata = product(tested_n,
                   tested_rank_deficiency)

@mark.parametrize("args", testdata)
def test_lu(args):

    n, rank_deficiency = args


    def check(A, x_expected, rank_expected, linearly_dependent_rows):
        b = A @ x_expected
        lu = LU(A)
        x = zeros(n)
        lu.solve(b, x)
        r = lu.rank()
        Q = lu.Q()

        set_printoptions(precision=6, linewidth=1000, suppress=True)
        assert r == rank_expected
        print()
        print(f"linearly_dependent_rows = {linearly_dependent_rows}")
        print(f"x = {x}")
        print(f"x[Q[r:]] = {x[Q[r:]]}")
        print()
        assert_allclose(A @ x, b)
        # assert all(isnan(x[linearly_dependent_rows]))
        # print(f"x = {x}")
        # print(f"x[linearly_dependent_rows] = {x[linearly_dependent_rows]}")

        # assert all(x[linearly_dependent_rows] == [])
        # assert linearly_dependent_rows == []


    x = linspace(1, n, n)

    linearly_dependent_rows = list(range(1, n, math.ceil(n / rank_deficiency))) \
        if rank_deficiency != 0 else []

    rank_expected = n - len(linearly_dependent_rows)

    A = matrix_non_singular(n)

    # Change the rows of A so that linearly dependent rows are produced
    for row in linearly_dependent_rows:
        A[row, :] = row * A[0, :]

    check(A, x, rank_expected, linearly_dependent_rows)

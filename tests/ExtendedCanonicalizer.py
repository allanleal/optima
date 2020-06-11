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
from numpy.linalg import norm, inv
from pytest import approx, mark
from itertools import product

from utils.matrices import testing_matrices_W


def check_canonical_form(canonicalizer, A, J):
    # Auxiliary varibles
    M = concatenate([A, J], 0)
    m, n = M.shape
    R = canonicalizer.R()
    Q = canonicalizer.Q()
    C = canonicalizer.C()

    # Check R*[A; J]*Q == C
    Cstar = R @ M[:,Q]

    assert allclose(Cstar, C)

    # Assemble Qtr, the transpose of the permutation matrix Q
    Qtr = arange(n)
    Qtr[Q] = arange(n)

    # Calculate the invR, the inverse of matrix R
    Rinv = inv(R)

    # Check inv(R) * C * tr(Q) == [A; J]
    Mstar = Rinv @ C[:, Qtr]

    assert allclose(Mstar, M)


def check_canonical_ordering(canonicalizer, weigths):
    n = canonicalizer.numVariables()
    nb = canonicalizer.numBasicVariables()
    nn = canonicalizer.numNonBasicVariables()
    ibasic = canonicalizer.indicesBasicVariables()
    inonbasic = canonicalizer.indicesNonBasicVariables()
    for i in range(1, nb):
        assert weigths[ibasic[i]] <= weigths[ibasic[i - 1]]
    for i in range(1, nn):
        assert weigths[inonbasic[i]] <= weigths[inonbasic[i - 1]]


def check_canonicalizer(canonicalizer, A, J):
    # Auxiliary variables
    n = canonicalizer.numVariables()
    m = canonicalizer.numEquations()
    nb = canonicalizer.numBasicVariables()

    #---------------------------------------------------------------------------
    # Check the computed canonical form with certain weights
    #---------------------------------------------------------------------------
    weigths = random.rand(n)

    canonicalizer.updateWithPriorityWeights(J, weigths)

    check_canonical_form(canonicalizer, A, J)

    #---------------------------------------------------------------------------
    # Set weights for the variables to update the basic/non-basic partition
    #---------------------------------------------------------------------------
    weigths = random.rand(n)

    Jnew = J + J

    canonicalizer.updateWithPriorityWeights(Jnew, weigths)

    check_canonical_form(canonicalizer, A, Jnew)

    check_canonical_ordering(canonicalizer, weigths)


# Tested cases for the matrix W
tested_matrices_W = testing_matrices_W

# Tested number of rows in matrix Au and Al (upper and lower blocks of A)
tested_mu = [7, 3]
tested_ml = [5, 1]

# Combination of all tested cases
testdata = product(tested_matrices_W,
                   tested_mu,
                   tested_ml)

@mark.parametrize("args", testdata)
def test_canonicalizer(args):

    set_printoptions(linewidth=100000, precision=6, floatmode='fixed', threshold=100000)


    assemble_W, mu, ml = args

    n = 15
    m = mu + ml

    W = assemble_W(m, n)

    A = W[:mu, :]
    J = W[mu:, :]

    canonicalizer = ExtendedCanonicalizer(A)
    check_canonicalizer(canonicalizer, A, J)
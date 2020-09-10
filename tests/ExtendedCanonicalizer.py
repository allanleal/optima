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
from numpy.testing import assert_array_almost_equal
from pytest import approx, mark
from itertools import product

from utils.matrices import testing_matrices_A, pascal_matrix


def check_canonical_form(canonicalizer, A, J):
    # Auxiliary varibles
    M = concatenate([A, J], 0)
    m, n = M.shape
    R = canonicalizer.R()
    Q = canonicalizer.Q()
    C = canonicalizer.C()

    # Check R*[A; J]*Q == C
    Cstar = R @ M[:,Q]

    assert_array_almost_equal(Cstar, C)


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


def check_new_ordering(canonicalizer, Kb, Kn):
    R = array(canonicalizer.R())  # create a copy of internal reference
    S = array(canonicalizer.S())  # create a copy of internal reference
    Q = array(canonicalizer.Q())  # create a copy of internal reference

    canonicalizer.updateOrdering(Kb, Kn)

    Rnew = canonicalizer.R()
    Snew = canonicalizer.S()
    Qnew = canonicalizer.Q()

    nb = len(Kb)

    assert_array_almost_equal(Rnew[:nb, :],  R[Kb, :])
    assert_array_almost_equal(Snew,  S[Kb, :][:, Kn])
    assert_array_almost_equal(Qnew[:nb],  Q[:nb][Kb])
    assert_array_almost_equal(Qnew[nb:],  Q[nb:][Kn])


def check_canonicalizer(canonicalizer, A, J):
    # Auxiliary variables
    n = canonicalizer.numVariables()
    m = canonicalizer.numEquations()

    #---------------------------------------------------------------------------
    # Check the computed canonical form with certain weights
    #---------------------------------------------------------------------------
    weigths = linspace(1, n, n)

    canonicalizer.updateWithPriorityWeights(J, weigths)
    canonicalizer.cleanResidualRoundoffErrors()

    check_canonical_form(canonicalizer, A, J)

    #---------------------------------------------------------------------------
    # Set weights for the variables to update the basic/non-basic partition
    #---------------------------------------------------------------------------
    weigths = linspace(n, 1, n)

    Jnew = J + J

    canonicalizer.updateWithPriorityWeights(Jnew, weigths)
    canonicalizer.cleanResidualRoundoffErrors()

    check_canonical_form(canonicalizer, A, Jnew)

    check_canonical_ordering(canonicalizer, weigths)

    #---------------------------------------------------------------------------
    # Check changing ordering of basic and non-basic variables work
    #---------------------------------------------------------------------------
    nb = canonicalizer.numBasicVariables()
    nn = canonicalizer.numNonBasicVariables()

    Kb = list(range(nb))
    Kn = list(range(nn))

    check_new_ordering(canonicalizer, Kb, Kn)  # identical ordering

    Kb = list(reversed(range(nb)))
    Kn = list(reversed(range(nn)))

    check_new_ordering(canonicalizer, Kb, Kn)  # reversed ordering


# Tested number of columns in W = [A; J]
tested_n = [15, 20, 30, 50]

# Tested number of rows in A
tested_mu = range(5, 15, 3)

# Tested number of rows in J
tested_ml = range(0, 5)

# Tested cases for the matrix W = [A; J]
tested_matrices_A = testing_matrices_A

# Combination of all tested cases
testdata = product(tested_n,
                   tested_mu,
                   tested_ml,
                   tested_matrices_A,)

@mark.parametrize("args", testdata)
def test_canonicalizer(args):

    set_printoptions(linewidth=100000, precision=6, floatmode='fixed', threshold=100000)


    n, mu, ml, assemble_A = args

    m = mu + ml

    # Skip tests in which there are more rows than columns
    if m > n: return

    A = assemble_A(mu, n)
    J = pascal_matrix(ml, n)

    #----------------------------------------------------------------------------------------------
    # WARNING!!
    #
    # ExtendedCanonicalizer assumes that rows in J are not linearly dependent
    # on rows in A. When testing, ensure this does not happen. This assumption
    # should be sensible in most application cases. However, when testing, it
    # may happen that W is produced so that this assumption is not respected.
    #----------------------------------------------------------------------------------------------

    canonicalizer = ExtendedCanonicalizer(A)
    canonicalizer.cleanResidualRoundoffErrors()
    check_canonicalizer(canonicalizer, A, J)

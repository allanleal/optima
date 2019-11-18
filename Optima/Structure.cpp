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

#include "Structure.hpp"

// Optima includes
#include <Optima/Exception.hpp>
#include <Optima/IndexUtils.hpp>

namespace Optima {

Structure::Structure(Index n)
: n(n), nlower(0), nupper(0), nfixed(0),
  lowerpartition(indices(n)),
  upperpartition(indices(n)),
  fixedpartition(indices(n))
{}

auto Structure::setObjectiveFunction(const ObjectiveFunction& objective_) -> void
{
    objective = objective_;
}

auto Structure::setEqualityConstraintMatrix(MatrixConstRef Ae_) -> void
{
    Assert(Ae_.cols() == n, "Could not set the equality constraint matrix.", "Mismatch number of columns and number of variables.");
    Assert(Ae_.rows() <= n, "Could not set the equality constraint matrix.", "More linear equality constraints than number of variables.");
    Ae = Ae_;
}

auto Structure::setInequalityConstraintMatrix(MatrixConstRef Ai_) -> void
{
    Assert(Ai_.cols() == n, "Could not set the inequality constraint matrix.", "Mismatch number of columns and number of variables.");
    Assert(Ai_.rows() <= n, "Could not set the inequality constraint matrix.", "More linear inequality constraints than number of variables.");
    Ai = Ai_;
}

auto Structure::setVariablesWithLowerBounds(IndicesConstRef inds) -> void
{
    nlower = inds.size();
    partitionRightStable(lowerpartition, inds);
}

auto Structure::allVariablesHaveLowerBounds() -> void
{
    nlower = n;
    lowerpartition = indices(n);
}

auto Structure::setVariablesWithUpperBounds(IndicesConstRef inds) -> void
{
    nupper = inds.size();
    partitionRightStable(upperpartition, inds);
}

auto Structure::allVariablesHaveUpperBounds() -> void
{
    nupper = n;
    upperpartition = indices(n);
}

auto Structure::setVariablesWithFixedValues(IndicesConstRef inds) -> void
{
    nfixed = inds.size();
    partitionRightStable(fixedpartition, inds);
}

auto Structure::numVariables() const -> Index
{
    return n;
}

auto Structure::numEqualityConstraints() const -> Index
{
    return Ae.rows();
}

auto Structure::numInequalityConstraints() const -> Index
{
    return Ai.rows();
}

auto Structure::objectiveFunction() const -> const ObjectiveFunction&
{
    return objective;
}

auto Structure::equalityConstraintMatrix() const -> MatrixConstRef
{
    return Ae;
}

auto Structure::inequalityConstraintMatrix() const -> MatrixConstRef
{
    return Ai;
}

auto Structure::variablesWithLowerBounds() const -> IndicesConstRef
{
    return lowerpartition.tail(nlower);
}

auto Structure::variablesWithUpperBounds() const -> IndicesConstRef
{
    return upperpartition.tail(nupper);
}

auto Structure::variablesWithFixedValues() const -> IndicesConstRef
{
    return fixedpartition.tail(nfixed);
}

auto Structure::variablesWithoutLowerBounds() const -> IndicesConstRef
{
    return lowerpartition.head(n - nlower);
}

auto Structure::variablesWithoutUpperBounds() const -> IndicesConstRef
{
    return upperpartition.head(n - nupper);
}

auto Structure::variablesWithoutFixedValues() const -> IndicesConstRef
{
    return fixedpartition.head(n - nfixed);
}

auto Structure::orderingLowerBounds() const -> IndicesConstRef
{
    return lowerpartition;
}

auto Structure::orderingUpperBounds() const -> IndicesConstRef
{
    return upperpartition;
}

auto Structure::orderingFixedValues() const -> IndicesConstRef
{
    return fixedpartition;
}

} // namespace Optima

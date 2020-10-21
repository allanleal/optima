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

#include "Stability2.hpp"

// C++ includes
#include <cassert>

// Optima includes
#include <Optima/IndexUtils.hpp>
#include <Optima/JacobianBlockW.hpp>

namespace Optima {

Stability2::Stability2(Index nx)
: jsu(indices(nx)), ns(nx), nlu(0), nuu(0), s(zeros(nx))
{}

auto Stability2::update(StabilityUpdateArgs args) -> void
{
    const auto [R, Sbn, Sbp, jb, jn] = args.W.canonicalForm();

    const auto gb = args.g(jb);
    const auto gn = args.g(jn);

    lambda = tr(R) * gb;

    s(jn) = gn - tr(Sbn) * gb;
    s(jb).fill(0.0);

    auto is_lower_unstable = [&](Index i) { return args.x[i] == args.xlower[i] && s[i] > 0.0; };
    auto is_upper_unstable = [&](Index i) { return args.x[i] == args.xupper[i] && s[i] < 0.0; };

    const auto nx = jsu.size();

    // Organize jsu = (js, jlu, juu) = (stable, lower unstable, upper unstable).
    const auto pos1 = moveRightIf(jsu, is_upper_unstable);
    const auto pos2 = moveRightIf(jsu.head(pos1), is_lower_unstable);

    ns  = pos2;
    nuu = nx - pos1;
    nlu = pos1 - pos2;
}

auto Stability2::status() const -> StabilityStatus
{
    const auto js = jsu.head(ns);
    const auto ju = jsu.tail(nlu + nuu);
    const auto jlu = ju.head(nlu);
    const auto juu = ju.tail(nuu);
    return {js, ju, jlu, juu, s, lambda};
}

} // namespace Optima

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

// pybind11 includes
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
namespace py = pybind11;

// Optima includes
#include <Optima/BasicSolver.hpp>
#include <Optima/Options.hpp>
#include <Optima/Result.hpp>
#include <Optima/Stability.hpp>
using namespace Optima;

void exportBasicSolver(py::module& m)
{
    auto init = [](Index n, Index m, MatrixConstRef4py A) -> BasicSolver
    {
        return BasicSolver({n, m, A});
    };

    auto solve = [](BasicSolver& self,
        const ObjectiveFunction4py& obj4py,
        const ConstraintFunction4py& h4py,
        VectorConstRef b,
        VectorConstRef xlower,
        VectorConstRef xupper,
        VectorRef x,
        VectorRef y,
        VectorRef z,
        Stability& stability) -> Result
    {
        auto obj = convert(obj4py);
        auto h = convert(h4py);
        return self.solve({ obj, h, b, xlower, xupper, x, y, z, stability });
    };

    Matrix tmp_dxdp, tmp_dydp, tmp_dzdp;
    auto sensitivities = [=](BasicSolver& self,
        MatrixConstRef4py dgdp,
        MatrixConstRef4py dhdp,
        MatrixConstRef4py dbdp,
        Stability const& stability,
        MatrixRef4py dxdp,
        MatrixRef4py dydp,
        MatrixRef4py dzdp) mutable
    {
        tmp_dxdp.resize(dxdp.rows(), dxdp.cols());
        tmp_dydp.resize(dydp.rows(), dydp.cols());
        tmp_dzdp.resize(dzdp.rows(), dzdp.cols());
        self.sensitivities({dgdp, dhdp, dbdp, stability, tmp_dxdp, tmp_dydp, tmp_dzdp});
        dxdp = tmp_dxdp;
        dydp = tmp_dydp;
        dzdp = tmp_dzdp;
    };

    py::class_<BasicSolver>(m, "BasicSolver")
        .def(py::init<>())
        .def(py::init(init))
        .def("setOptions", &BasicSolver::setOptions)
        .def("solve", solve)
        .def("sensitivities", sensitivities)
        ;
}
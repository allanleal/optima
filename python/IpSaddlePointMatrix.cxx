// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
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
namespace py = pybind11;

// Optima includes
#include <Optima/IpSaddlePointMatrix.hpp>
using namespace Optima;

void exportIpSaddlePointMatrix(py::module& m)
{
    py::class_<IpSaddlePointMatrix>(m, "IpSaddlePointMatrix")
        .def_readonly("H", &IpSaddlePointMatrix::H)
        .def_readonly("A", &IpSaddlePointMatrix::A)
        .def_readonly("Z", &IpSaddlePointMatrix::Z)
        .def_readonly("W", &IpSaddlePointMatrix::W)
        .def_readonly("L", &IpSaddlePointMatrix::L)
        .def_readonly("U", &IpSaddlePointMatrix::U)
        .def_readonly("nf", &IpSaddlePointMatrix::nf)
        .def(py::init<MatrixConstRef, MatrixConstRef, VectorConstRef, VectorConstRef, VectorConstRef, VectorConstRef, Index>(), py::arg("H"), py::arg("A"), py::arg("Z"), py::arg("W"), py::arg("L"), py::arg("U"), py::arg("nf"))
        .def("array", [](IpSaddlePointMatrix self) { return Matrix(self); })
        ;

    py::class_<IpSaddlePointVector>(m, "IpSaddlePointVector")
        .def_readonly("a", &IpSaddlePointVector::a)
        .def_readonly("b", &IpSaddlePointVector::b)
        .def_readonly("c", &IpSaddlePointVector::c)
        .def_readonly("d", &IpSaddlePointVector::d)
        .def(py::init<VectorConstRef, VectorConstRef, VectorConstRef, VectorConstRef>(), py::arg("a"), py::arg("b"), py::arg("c"), py::arg("d"))
        .def(py::init<VectorConstRef, Index, Index>(), py::arg("r"), py::arg("n"), py::arg("m"))
        .def("array", [](IpSaddlePointVector self) { return Vector(self); })
        ;

    py::class_<IpSaddlePointSolution>(m, "IpSaddlePointSolution")
        .def_readwrite("x", &IpSaddlePointSolution::x)
        .def_readwrite("y", &IpSaddlePointSolution::y)
        .def_readwrite("z", &IpSaddlePointSolution::z)
        .def_readwrite("w", &IpSaddlePointSolution::w)
        .def(py::init<VectorRef, VectorRef, VectorRef, VectorRef>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def(py::init<VectorRef, Index, Index>(), py::arg("s"), py::arg("n"), py::arg("m"))
        .def("array", [](IpSaddlePointSolution self) { return Vector(self); })
        ;
}

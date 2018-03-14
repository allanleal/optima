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
#include <Optima/SaddlePointMatrix.hpp>
using namespace Optima;

void exportSaddlePointMatrix(py::module& m)
{
    py::class_<SaddlePointMatrix>(m, "SaddlePointMatrix")
        .def_readonly("H", &SaddlePointMatrix::H)
        .def_readonly("A", &SaddlePointMatrix::A)
        .def_readonly("G", &SaddlePointMatrix::G)
        .def_readonly("nx", &SaddlePointMatrix::nx)
        .def_readonly("nf", &SaddlePointMatrix::nf)
        .def(py::init<MatrixXdConstRef, MatrixXdConstRef, Index, Index>(), py::arg("H"), py::arg("A"), py::arg("nx"), py::arg("nf") = 0)
        .def(py::init<MatrixXdConstRef, MatrixXdConstRef, MatrixXdConstRef, Index, Index>(), py::arg("H"), py::arg("A"), py::arg("G"), py::arg("nx"), py::arg("nf") = 0)
        .def("array", [](SaddlePointMatrix self) { return MatrixXd(self); })
        ;

    py::class_<SaddlePointVector>(m, "SaddlePointVector")
        .def_readonly("a", &SaddlePointVector::a)
        .def_readonly("b", &SaddlePointVector::b)
        .def(py::init<VectorXdConstRef, VectorXdConstRef>(), py::arg("a"), py::arg("b"))
        .def(py::init<VectorXdConstRef, Index, Index>(), py::arg("r"), py::arg("n"), py::arg("m"))
        .def("array", [](SaddlePointVector self) { return VectorXd(self); })
        ;

    py::class_<SaddlePointSolution>(m, "SaddlePointSolution")
        .def_readwrite("x", &SaddlePointSolution::x)
        .def_readwrite("y", &SaddlePointSolution::y)
        .def(py::init<VectorXdRef, VectorXdRef>(), py::arg("x"), py::arg("y"))
        .def(py::init<VectorXdRef, Index, Index>(), py::arg("s"), py::arg("n"), py::arg("m"))
        .def("array", [](SaddlePointSolution self) { return VectorXd(self); })
        ;
}
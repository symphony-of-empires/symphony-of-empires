// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      pyvm.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <unordered_map>
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"
#include "eng3d/ui/components.hpp"

#ifdef E3D_FEATURE_PYTHON

#include "eng3d/pyvm.hpp"
#define PY_SSIZE_T_CLEAN
#define Py_LIMITED_API
#include <Python.h>

std::unordered_map<int, UI::Widget*> python_widgets;
std::unordered_map<int, std::shared_ptr<Eng3D::Texture>> python_textures;
std::unordered_map<std::string, int> python_ui_callbacks; 

Eng3D::PythonVM::PythonVM(Eng3D::State& _s)
    : s{ _s }
{
    if(PyImport_AppendInittab("eng3d", &[]() -> PyObject* {
        static PyMethodDef eng3d_methods[] = {
            { "numargs", [](PyObject* self, PyObject* args) -> PyObject* {
                return PyLong_FromLong(10);
            }, METH_VARARGS, "Return the number of arguments received by the process." },
            {nullptr, nullptr, 0, nullptr}
        };
        static PyModuleDef eng3d_module = {
            PyModuleDef_HEAD_INIT, "eng3d", nullptr, -1, eng3d_methods,
            nullptr, nullptr, nullptr, nullptr
        };
        auto* module_obj = PyModule_Create(&eng3d_module);
        auto* module_dict = PyImport_GetModuleDict();

        static PyMethodDef eng3d_core_methods[] = {
            { "numargs", [](PyObject* self, PyObject* args) -> PyObject* {
                return PyLong_FromLong(10);
            }, METH_VARARGS, "Return the number of arguments received by the process." },
            {nullptr, nullptr, 0, nullptr}
        };
        static PyModuleDef eng3d_core_module = {
            PyModuleDef_HEAD_INIT, "eng3d", nullptr, -1, eng3d_core_methods,
            nullptr, nullptr, nullptr, nullptr
        };
        auto* core_module_obj = PyModule_Create(&eng3d_core_module);
        PyDict_SetItemString(module_dict, "eng3d.core", core_module_obj);

        static PyMethodDef eng3d_ui_methods[] = {
            { "numargs", [](PyObject* self, PyObject* args) -> PyObject* {
                return PyLong_FromLong(10);
            }, METH_VARARGS, "Return the number of arguments received by the process." },
            {nullptr, nullptr, 0, nullptr}
        };
        static PyModuleDef eng3d_ui_module = {
            PyModuleDef_HEAD_INIT, "eng3d", nullptr, -1, eng3d_ui_methods,
            nullptr, nullptr, nullptr, nullptr
        };
        auto* ui_module_obj = PyModule_Create(&eng3d_ui_module);
        PyDict_SetItemString(module_dict, "eng3d.ui", ui_module_obj);
        return module_obj;
    }) < 0) {
        PyErr_Print();
        CXX_THROW(Eng3D::PythonException, translate("Failed to add eng3d core module"));
    }

    Py_Initialize();
}

void Eng3D::PythonVM::run_string(const std::string_view name, const std::string_view body) {
    auto* code = Py_CompileString(body.data(), "test", Py_file_input);
    auto* main_module = PyImport_AddModule("__main__");
    auto* global_dict = PyModule_GetDict(main_module);
    auto* local_dict = PyDict_New();
    auto* obj = PyEval_EvalCode(code, global_dict, local_dict);
    if(obj == nullptr) {
        PyErr_Print();
        CXX_THROW(Eng3D::PythonException, translate_format("Failed to execute \"%s\"", name.data()));
    }
}

void Eng3D::PythonVM::add_module(const std::string_view path) {
    auto* name = PyUnicode_DecodeFSDefault(path.data());
    // Error checking of pName left out
    auto* py_module = PyImport_Import(name);
    Py_XDECREF(name);
    if(py_module == nullptr) {
        PyErr_Print();
        CXX_THROW(Eng3D::PythonException, translate_format("Failed to load \"%s\"", path.data()));
    }
    modules.emplace_back(py_module);

    auto py_func = PyObject_GetAttrString(py_module, "__init__");
    // func is a new reference
    if(py_func && PyCallable_Check(py_func)) {
        auto py_value = PyObject_CallObject(py_func, nullptr);
        Py_XDECREF(py_value);
    } else {
        if(PyErr_Occurred())
            PyErr_Print();
        CXX_THROW(Eng3D::PythonException, translate_format("Cannot find function \"%s\"", "__init__"));
    }
    Py_XDECREF(py_func);
}

Eng3D::PythonVM::~PythonVM() {
    Py_Finalize();
}

Eng3D::PythonObj::PythonObj(void* _obj)
    : obj{ _obj }
{
    Py_XINCREF(this->obj);
}

Eng3D::PythonObj::PythonObj(const PythonObj& rhs) {
    this->obj = rhs.obj;
    Py_XINCREF(this->obj);
}

Eng3D::PythonObj::PythonObj(PythonObj&& rhs) noexcept {
    this->obj = rhs.obj;
    rhs.obj = nullptr;
}

Eng3D::PythonObj::~PythonObj()
{
    Py_XDECREF(this->obj);
}

#endif

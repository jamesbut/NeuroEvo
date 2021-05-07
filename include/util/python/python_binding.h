#ifndef _PYTHON_BINDING_H_
#define _PYTHON_BINDING_H_

#include <string>
#include <sstream>
#include <Python.h>
#include <iostream>
#include <vector>

namespace NeuroEvo {

class PythonModule
{

public:

    PythonModule(const std::string& module_name, const std::string& module_path) :
        _module_name(module_name),
        _module_path(module_path) {}

    void initialise()
    {
        //Initialise python interpreter
        Py_Initialize();

        //Add correct path to python interpreter
        std::string import_command = "import sys; sys.path.insert(0, '" + _module_path
            + "')";
        PyRun_SimpleString(import_command.c_str());

        //Load the module object
        PyObject* module_name = PyUnicode_FromString((char*)_module_name.c_str());
        PyObject* module = PyImport_Import(module_name);

        if(module == NULL)
            PyErr_Print();

        //Turn module into dictionary
        _module_dict = PyModule_GetDict(module);
    }

    template <typename... T, typename... Args>
    std::tuple<T...> call_function(const std::string& func_name, Args... args)
    {
        PyObject* func_return = send_function(func_name, args...);

        //Check to see whether return tuple is empty
        if constexpr (!(std::is_same<T, void>::value && ...))
        {
            std::cout << "Returning val from call function" << std::endl;
            //TODO: Change to make tuple?
            std::tuple<T...> return_vals{(convert_py_object<T>(func_return), ...)};

            return return_vals;
        } else
        {
            std::cout << "Not returning anything from call function!" << std::endl;
            //TODO: Change to make tuple?
            std::tuple<T...> return_vals;
            return return_vals;
        }
    }

private:

    template <typename... Args>
    PyObject* send_function(const std::string& func_name, Args... args)
    {
        const unsigned num_args = sizeof...(args);

        PyObject* py_args = PyTuple_New(num_args);

        std::vector<PyObject*> arg_objs;
        (arg_objs.push_back(convert_to_py_object(args)), ...);

        for(unsigned i = 0; i < num_args; i++)
            PyTuple_SetItem(py_args, i, arg_objs[i]);

        PyObject* func_obj = PyDict_GetItemString(_module_dict, func_name.c_str());
        PyObject* func_return;

        if(func_obj == NULL)
        {
            std::cout << "Not function!!" << std::endl;
            PyErr_Print();
        } else
            std::cout << "Function :)" << std::endl;

        if(PyCallable_Check(func_obj))
        {
            std::cout << "Callable function :)" << std::endl;
            func_return = PyObject_CallObject(func_obj, py_args);
        }
        else
        {
            std::cout << "Not callable functions :(" << std::endl;
            PyErr_Print();
        }

        if(func_return == NULL)
        {
            std::cout << "Function return is null!" << std::endl;
            PyErr_Print();
        }

        return func_return;

    }


    //TODO: Redo this as specific templates like the return types
    template <typename T>
    PyObject* convert_to_py_object(T args) const
    {
        if constexpr (std::is_same<T, int>::value)
        {
            std::cout << "To long.." << std::endl;
            return PyLong_FromLong(args);
        } else if constexpr (std::is_same<T, double>::value)
        {
            std::cout << "To float.." << std::endl;
            return PyFloat_FromDouble(args);
        } else if constexpr (std::is_same<T, std::string>::value)
        {
            std::cout << "To string from string.." << std::endl;
            return PyUnicode_FromString(args);
        } else if constexpr (std::is_same<T, char*>::value)
        {
            std::cout << "To string from char*.." << std::endl;
            return PyUnicode_FromString(args);
        } else if constexpr (std::is_same<T, const char*>::value)
        {
            std::cout << "To string from const char*.." << std::endl;
            return PyUnicode_FromString(args);
        } else
        {
            std::cerr << "Could not convert to PyObject!!" << std::endl;
            return new PyObject;
        }
    }

    template <typename T>
    T convert_py_object(PyObject* py_object) const;

    template <>
    int convert_py_object(PyObject* py_object) const
    {
        return PyLong_AsLong(py_object);
    }

    template <>
    const int convert_py_object(PyObject* py_object) const
    {
        return PyLong_AsLong(py_object);
    }

    template <>
    unsigned convert_py_object(PyObject* py_object) const
    {
        return PyLong_AsLong(py_object);
    }

    template <>
    const unsigned convert_py_object(PyObject* py_object) const
    {
        return PyLong_AsLong(py_object);
    }

    template <>
    double convert_py_object(PyObject* py_object) const
    {
        return PyFloat_AsDouble(py_object);
    }

    template <>
    const double convert_py_object(PyObject* py_object) const
    {
        return PyFloat_AsDouble(py_object);
    }

    template <>
    float convert_py_object(PyObject* py_object) const
    {
        return PyFloat_AsDouble(py_object);
    }

    template <>
    const float convert_py_object(PyObject* py_object) const
    {
        return PyFloat_AsDouble(py_object);
    }

    template <>
    std::vector<double> convert_py_object(PyObject* py_object) const
    {
        PyObject* py_tuple = PyList_AsTuple(py_object);
        const unsigned tup_size = PyObject_Length(py_tuple);
        std::vector<double> list_vec(tup_size);
        for(unsigned i = 0; i < tup_size; i++)
            list_vec[i] = convert_py_object<double>(PyTuple_GetItem(py_tuple, i));
        return list_vec;
    }

    const std::string _module_name;
    const std::string _module_path;

    PyObject* _module_dict;

};

} // namespace NeuroEvo

#endif

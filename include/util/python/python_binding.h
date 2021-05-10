#ifndef _PYTHON_BINDING_H_
#define _PYTHON_BINDING_H_

/*
 * This python binding provides an abstract interface through which one can call
 * arbitrary python functions from a python module.
 * I have not accounted for memory leaks, if there are memory leaks this binding
 * could be the culprit - just need to add more Py_DECREF()
 */

#include <string>
#include <sstream>
#include <Python.h>
#include <iostream>
#include <vector>
#include <queue>

namespace NeuroEvo {

class PythonModule
{

public:

    PythonModule(const std::string& module_name, const std::string& module_path) :
        _module_name(module_name),
        _module_path(module_path) {}

    //Load python module
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

        Py_DECREF(module_name);

        if(module == NULL)
            PyErr_Print();

        //Turn module into dictionary
        _module_dict = PyModule_GetDict(module);

        Py_DECREF(module);
    }


    template <typename... T, typename... Args>
    std::tuple<T...> call_function(const std::string& func_name,
                                   const Args&... args) const
    {
        //Send function and receive result
        ResultFeeder return_feeder = ResultFeeder(send_function(func_name, args...));

        //Check to see whether return tuple is empty
        if constexpr (!(std::is_same<T, void>::value && ...))
            //Build tuple from converted python objects
            return std::make_tuple<T...>(Converter<T>::convert(return_feeder.next())...);
        else
            return std::make_tuple();
    }

private:

    template <typename... Args>
    PyObject* send_function(const std::string& func_name, const Args&... args) const
    {
        const unsigned num_args = sizeof...(args);

        //Convert args to PyObjects
        PyObject* py_args = PyTuple_New(num_args);
        std::vector<PyObject*> arg_objs{Converter<Args>::convert(args)...};
        for(unsigned i = 0; i < num_args; i++)
            PyTuple_SetItem(py_args, i, arg_objs[i]);

        PyObject* func_obj = PyDict_GetItemString(_module_dict, func_name.c_str());
        PyObject* func_return;

        if(func_obj == NULL)
        {
            std::cerr << "Function name: " << func_name << std::endl;
            std::cerr << "Not a function!!" << std::endl;
            PyErr_Print();
            exit(0);
        }

        //Call function
        if(PyCallable_Check(func_obj))
            func_return = PyObject_CallObject(func_obj, py_args);
        else
        {
            std::cerr << "Function name: " << func_name << std::endl;
            std::cerr << "Not a callable function" << std::endl;
            PyErr_Print();
            exit(0);
        }

        if(func_return == NULL)
        {
            std::cout << "Function return is null!" << std::endl;
            PyErr_Print();
            exit(0);
        }

        return func_return;

    }

    //Processes the returns from Python function calls
    class ResultFeeder
    {

    public:

        ResultFeeder(PyObject* return_result) :
            _return_results(queue_returns(return_result)) {}

        //Returns next result
        PyObject* next()
        {
            PyObject* next_result = _return_results.front();
            _return_results.pop();
            return next_result;
        }

    private:

        //Queues return value(s)
        std::queue<PyObject*> queue_returns(PyObject* return_results)
        {
            std::queue<PyObject*> return_queue;

            //If return type is tuple, split up and queue elements
            if(PyTuple_Check(return_results))
            {
                const unsigned tup_size = PyObject_Length(return_results);
                for(unsigned i = 0; i < tup_size; i++)
                    return_queue.push(PyTuple_GetItem(return_results, i));

            } else
                return_queue.push(return_results);

            return return_queue;

        }

        std::queue<PyObject*> _return_results;

    };

    //Can't do partial specialisation on functions so I used this struct workaround
    //I need the partial specialisation for the vector<T> implementation
    template <typename T>
    struct Converter;

    template <>
    struct Converter<int> {
        static int convert(PyObject* py_object) {
            return PyLong_AsLong(py_object);
        }
    };

    template <>
    struct Converter<const int> {
        static const int convert(PyObject* py_object) {
            return PyLong_AsLong(py_object);
        }
    };

    template <>
    struct Converter<unsigned> {
        static unsigned convert(PyObject* py_object) {
            if(!PyLong_Check(py_object))
            {
                std::cerr << "Trying to convert py object to an unsigned int " <<
                    "that is not an unsigned int! " << std::endl;
                exit(0);

            }
            return PyLong_AsLong(py_object);
        }
        static PyObject* convert(unsigned v) {
            return PyLong_FromLong(v);
        }
    };

    template <>
    struct Converter<const unsigned> {
        static const unsigned convert(PyObject* py_object) {
            return PyLong_AsLong(py_object);
        }
    };

    template <>
    struct Converter<double> {
        static double convert(PyObject* py_object) {
            return PyFloat_AsDouble(py_object);
        }
    };

    template <>
    struct Converter<const double> {
        static const double convert(PyObject* py_object) {
            return PyFloat_AsDouble(py_object);
        }
    };

    template <>
    struct Converter<float> {
        static float convert(PyObject* py_object) {
            return PyFloat_AsDouble(py_object);
        }
    };

    template <>
    struct Converter<const float> {
        static const float convert(PyObject* py_object) {
            return PyFloat_AsDouble(py_object);
        }
    };

    template <>
    struct Converter<const char*> {
        static PyObject* convert(const char* v) {
            return PyUnicode_FromString(v);
        }
    };

    template <>
    struct Converter<const std::string> {
        static PyObject* convert(const std::string v) {
            return PyUnicode_FromString(v.c_str());
        }
    };

    template <>
    struct Converter<std::string> {
        static PyObject* convert(std::string v) {
            return PyUnicode_FromString(v.c_str());
        }
    };

    template <>
    struct Converter<bool> {
        static bool convert(PyObject* py_object) {
            return PyLong_AsLong(py_object);
        }
        static PyObject* convert(bool v) {
            return v ? Py_True : Py_False;
        }
    };

    template <typename T>
    struct Converter<std::vector<T>>
    {
        static std::vector<T> convert(PyObject* py_object)
        {
            if(!PyList_Check(py_object))
            {
                std::cerr << "Trying to convert py object to a vector that is not a" <<
                    " list!";
                exit(0);

            }
            PyObject* py_tuple = PyList_AsTuple(py_object);
            const unsigned tup_size = PyObject_Length(py_tuple);
            std::vector<T> list_vec(tup_size);
            for(unsigned i = 0; i < tup_size; i++)
                list_vec[i] = Converter<T>::convert(PyTuple_GetItem(py_tuple, i));
            return list_vec;
        }
    };

    const std::string _module_name;
    const std::string _module_path;

    PyObject* _module_dict;

};

} // namespace NeuroEvo

#endif

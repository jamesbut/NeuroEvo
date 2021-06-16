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
#include <util/python/python_converter.h>

namespace NeuroEvo {

class PythonModule
{

public:

    PythonModule(const std::string& module_name, const std::string& module_path) :
        _module_name(module_name),
        _module_path(module_path) {}

    ~PythonModule()
    {
	Py_DECREF(_module_dict);
    }	

    //Load python module
    void initialise()
    {
        //Initialise python interpreter
        Py_Initialize();

        //Add module path to python interpreter
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
        PyObject* func_return = send_function(func_name, args...);
        ResultFeeder return_feeder = ResultFeeder(func_return);

        //Check to see whether return tuple is empty
        if constexpr (!(std::is_same<T, void>::value && ...))
        {
            //Build tuple from converted python objects
            std::tuple<T...> return_tuple{Converter<T>::convert(return_feeder.next())...};
            Py_DECREF(func_return);
            return return_tuple;
        }
        else
            return std::make_tuple();
    }

private:

    template <typename... Args>
    PyObject* send_function(const std::string& func_name, const Args&... args) const
    {
        //Convert arguments to appropriate PyObjects
        std::vector<PyObject*> arg_objs{Converter<Args>::convert(args)...};

        //Pull dictionary out and assume this is the kwargs
        PyObject* py_kwargs = NULL;
        for(auto it = arg_objs.begin(); it != arg_objs.end(); it++)
            if(PyDict_Check(*it))
            {
                py_kwargs = *it;
                arg_objs.erase(it);
                break;
            }

        //Put the rest of the arguments in a tuple
        PyObject* py_args = PyTuple_New(arg_objs.size());
        for(std::size_t i = 0; i < arg_objs.size(); i++)
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
            func_return = PyObject_Call(func_obj, py_args, py_kwargs);
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

	if(py_kwargs != NULL)
	    Py_DECREF(py_kwargs);
	if(py_args != NULL)
	    Py_DECREF(py_args);

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

    const std::string _module_name;
    const std::string _module_path;

    PyObject* _module_dict;

};

} // namespace NeuroEvo

#endif

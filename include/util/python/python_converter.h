#ifndef _PYTHON_CONVERTER_H_
#define _PYTHON_CONVERTER_H_

/*
 * Converter structs that define type conversions between C++ and python
 */

#include <Python.h>
#include <iostream>
#include <map>

namespace NeuroEvo {

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
static PyObject* convert(double v) {
    return PyFloat_FromDouble(v);
}
};

template <>
struct Converter<const double> {
static const double convert(PyObject* py_object) {
    return PyFloat_AsDouble(py_object);
}
static PyObject* convert(const double v) {
    return PyFloat_FromDouble(v);
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
    //return v ? Py_True : Py_False;
    return v ? PyBool_FromLong(1) : PyBool_FromLong(0);
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
    Py_DECREF(py_tuple);
    return list_vec;
}

static PyObject* convert(std::vector<T> v)
{
    PyObject* py_list = PyList_New(v.size());
    for(std::size_t i = 0; i < v.size(); i++)
	PyList_SET_ITEM(py_list, i, Converter<T>::convert(v[i]));
    return py_list;
}
};

template <typename T>
struct Converter<std::map<std::string, T>>
{
//Convert map to python dictionary
static PyObject* convert(std::map<std::string, T> v) {
    PyObject* py_dict = PyDict_New();
    for(auto& item : v)
	PyDict_SetItem(py_dict,
		       Converter<std::string>::convert(item.first),
		       Converter<T>::convert(item.second));
    return py_dict;
}
};

//TODO: Find some way to remove duplicate code for const versions
template <typename T>
struct Converter<std::map<const std::string, const T>>
{
//Convert map to python dictionary
static PyObject* convert(std::map<const std::string, const T> v) {
    PyObject* py_dict = PyDict_New();
    for(auto& item : v)
	PyDict_SetItem(py_dict,
		       Converter<const std::string>::convert(item.first),
		       Converter<const T>::convert(item.second));
    return py_dict;
}
};

} // namespace NeuroEvo

#endif

// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from custom_msgs:msg/GuiCommand.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "custom_msgs/msg/detail/gui_command__struct.h"
#include "custom_msgs/msg/detail/gui_command__functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool custom_msgs__msg__gui_command__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[40];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("custom_msgs.msg._gui_command.GuiCommand", full_classname_dest, 39) == 0);
  }
  custom_msgs__msg__GuiCommand * ros_message = _ros_message;
  {  // nav_mode
    PyObject * field = PyObject_GetAttrString(_pymsg, "nav_mode");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->nav_mode = (int8_t)PyLong_AsLong(field);
    Py_DECREF(field);
  }
  {  // goal_lat
    PyObject * field = PyObject_GetAttrString(_pymsg, "goal_lat");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->goal_lat = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // goal_lon
    PyObject * field = PyObject_GetAttrString(_pymsg, "goal_lon");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->goal_lon = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // target_cone_id
    PyObject * field = PyObject_GetAttrString(_pymsg, "target_cone_id");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->target_cone_id = (int32_t)PyLong_AsLong(field);
    Py_DECREF(field);
  }
  {  // set_search_skew
    PyObject * field = PyObject_GetAttrString(_pymsg, "set_search_skew");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->set_search_skew = (Py_True == field);
    Py_DECREF(field);
  }
  {  // search_skew
    PyObject * field = PyObject_GetAttrString(_pymsg, "search_skew");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->search_skew = (int8_t)PyLong_AsLong(field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * custom_msgs__msg__gui_command__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of GuiCommand */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("custom_msgs.msg._gui_command");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "GuiCommand");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  custom_msgs__msg__GuiCommand * ros_message = (custom_msgs__msg__GuiCommand *)raw_ros_message;
  {  // nav_mode
    PyObject * field = NULL;
    field = PyLong_FromLong(ros_message->nav_mode);
    {
      int rc = PyObject_SetAttrString(_pymessage, "nav_mode", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // goal_lat
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->goal_lat);
    {
      int rc = PyObject_SetAttrString(_pymessage, "goal_lat", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // goal_lon
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->goal_lon);
    {
      int rc = PyObject_SetAttrString(_pymessage, "goal_lon", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // target_cone_id
    PyObject * field = NULL;
    field = PyLong_FromLong(ros_message->target_cone_id);
    {
      int rc = PyObject_SetAttrString(_pymessage, "target_cone_id", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // set_search_skew
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->set_search_skew ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "set_search_skew", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // search_skew
    PyObject * field = NULL;
    field = PyLong_FromLong(ros_message->search_skew);
    {
      int rc = PyObject_SetAttrString(_pymessage, "search_skew", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}

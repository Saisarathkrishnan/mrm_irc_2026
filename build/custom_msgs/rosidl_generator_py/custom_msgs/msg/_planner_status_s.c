// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from custom_msgs:msg/PlannerStatus.idl
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
#include "custom_msgs/msg/detail/planner_status__struct.h"
#include "custom_msgs/msg/detail/planner_status__functions.h"

ROSIDL_GENERATOR_C_IMPORT
bool builtin_interfaces__msg__time__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * builtin_interfaces__msg__time__convert_to_py(void * raw_ros_message);

ROSIDL_GENERATOR_C_EXPORT
bool custom_msgs__msg__planner_status__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[46];
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
    assert(strncmp("custom_msgs.msg._planner_status.PlannerStatus", full_classname_dest, 45) == 0);
  }
  custom_msgs__msg__PlannerStatus * ros_message = _ros_message;
  {  // state
    PyObject * field = PyObject_GetAttrString(_pymsg, "state");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->state = (int8_t)PyLong_AsLong(field);
    Py_DECREF(field);
  }
  {  // nav_mode
    PyObject * field = PyObject_GetAttrString(_pymsg, "nav_mode");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->nav_mode = (int8_t)PyLong_AsLong(field);
    Py_DECREF(field);
  }
  {  // curr_lat
    PyObject * field = PyObject_GetAttrString(_pymsg, "curr_lat");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->curr_lat = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // curr_lon
    PyObject * field = PyObject_GetAttrString(_pymsg, "curr_lon");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->curr_lon = PyFloat_AS_DOUBLE(field);
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
  {  // distance_to_goal_m
    PyObject * field = PyObject_GetAttrString(_pymsg, "distance_to_goal_m");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->distance_to_goal_m = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // current_yaw_deg
    PyObject * field = PyObject_GetAttrString(_pymsg, "current_yaw_deg");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->current_yaw_deg = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // target_yaw_deg
    PyObject * field = PyObject_GetAttrString(_pymsg, "target_yaw_deg");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->target_yaw_deg = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // heading_error_deg
    PyObject * field = PyObject_GetAttrString(_pymsg, "heading_error_deg");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->heading_error_deg = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // cone_detected
    PyObject * field = PyObject_GetAttrString(_pymsg, "cone_detected");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->cone_detected = (Py_True == field);
    Py_DECREF(field);
  }
  {  // cone_x
    PyObject * field = PyObject_GetAttrString(_pymsg, "cone_x");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->cone_x = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // cone_y
    PyObject * field = PyObject_GetAttrString(_pymsg, "cone_y");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->cone_y = PyFloat_AS_DOUBLE(field);
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
  {  // obstacle_detected
    PyObject * field = PyObject_GetAttrString(_pymsg, "obstacle_detected");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->obstacle_detected = (Py_True == field);
    Py_DECREF(field);
  }
  {  // gps_goal_reached
    PyObject * field = PyObject_GetAttrString(_pymsg, "gps_goal_reached");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->gps_goal_reached = (Py_True == field);
    Py_DECREF(field);
  }
  {  // cone_goal_reached
    PyObject * field = PyObject_GetAttrString(_pymsg, "cone_goal_reached");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->cone_goal_reached = (Py_True == field);
    Py_DECREF(field);
  }
  {  // autonomous_enabled
    PyObject * field = PyObject_GetAttrString(_pymsg, "autonomous_enabled");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->autonomous_enabled = (Py_True == field);
    Py_DECREF(field);
  }
  {  // stamp
    PyObject * field = PyObject_GetAttrString(_pymsg, "stamp");
    if (!field) {
      return false;
    }
    if (!builtin_interfaces__msg__time__convert_from_py(field, &ros_message->stamp)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * custom_msgs__msg__planner_status__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of PlannerStatus */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("custom_msgs.msg._planner_status");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "PlannerStatus");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  custom_msgs__msg__PlannerStatus * ros_message = (custom_msgs__msg__PlannerStatus *)raw_ros_message;
  {  // state
    PyObject * field = NULL;
    field = PyLong_FromLong(ros_message->state);
    {
      int rc = PyObject_SetAttrString(_pymessage, "state", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
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
  {  // curr_lat
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->curr_lat);
    {
      int rc = PyObject_SetAttrString(_pymessage, "curr_lat", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // curr_lon
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->curr_lon);
    {
      int rc = PyObject_SetAttrString(_pymessage, "curr_lon", field);
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
  {  // distance_to_goal_m
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->distance_to_goal_m);
    {
      int rc = PyObject_SetAttrString(_pymessage, "distance_to_goal_m", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // current_yaw_deg
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->current_yaw_deg);
    {
      int rc = PyObject_SetAttrString(_pymessage, "current_yaw_deg", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // target_yaw_deg
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->target_yaw_deg);
    {
      int rc = PyObject_SetAttrString(_pymessage, "target_yaw_deg", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // heading_error_deg
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->heading_error_deg);
    {
      int rc = PyObject_SetAttrString(_pymessage, "heading_error_deg", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // cone_detected
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->cone_detected ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "cone_detected", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // cone_x
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->cone_x);
    {
      int rc = PyObject_SetAttrString(_pymessage, "cone_x", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // cone_y
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->cone_y);
    {
      int rc = PyObject_SetAttrString(_pymessage, "cone_y", field);
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
  {  // obstacle_detected
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->obstacle_detected ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "obstacle_detected", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // gps_goal_reached
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->gps_goal_reached ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "gps_goal_reached", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // cone_goal_reached
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->cone_goal_reached ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "cone_goal_reached", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // autonomous_enabled
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->autonomous_enabled ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "autonomous_enabled", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // stamp
    PyObject * field = NULL;
    field = builtin_interfaces__msg__time__convert_to_py(&ros_message->stamp);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "stamp", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}

# generated from rosidl_generator_py/resource/_idl.py.em
# with input from custom_msgs:msg/PlannerStatus.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_PlannerStatus(type):
    """Metaclass of message 'PlannerStatus'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('custom_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'custom_msgs.msg.PlannerStatus')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__planner_status
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__planner_status
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__planner_status
            cls._TYPE_SUPPORT = module.type_support_msg__msg__planner_status
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__planner_status

            from builtin_interfaces.msg import Time
            if Time.__class__._TYPE_SUPPORT is None:
                Time.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class PlannerStatus(metaclass=Metaclass_PlannerStatus):
    """Message class 'PlannerStatus'."""

    __slots__ = [
        '_state',
        '_nav_mode',
        '_curr_lat',
        '_curr_lon',
        '_goal_lat',
        '_goal_lon',
        '_distance_to_goal_m',
        '_current_yaw_deg',
        '_target_yaw_deg',
        '_heading_error_deg',
        '_cone_detected',
        '_cone_x',
        '_cone_y',
        '_target_cone_id',
        '_obstacle_detected',
        '_gps_goal_reached',
        '_cone_goal_reached',
        '_autonomous_enabled',
        '_stamp',
    ]

    _fields_and_field_types = {
        'state': 'int8',
        'nav_mode': 'int8',
        'curr_lat': 'double',
        'curr_lon': 'double',
        'goal_lat': 'double',
        'goal_lon': 'double',
        'distance_to_goal_m': 'double',
        'current_yaw_deg': 'double',
        'target_yaw_deg': 'double',
        'heading_error_deg': 'double',
        'cone_detected': 'boolean',
        'cone_x': 'double',
        'cone_y': 'double',
        'target_cone_id': 'int32',
        'obstacle_detected': 'boolean',
        'gps_goal_reached': 'boolean',
        'cone_goal_reached': 'boolean',
        'autonomous_enabled': 'boolean',
        'stamp': 'builtin_interfaces/Time',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('int8'),  # noqa: E501
        rosidl_parser.definition.BasicType('int8'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('int32'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['builtin_interfaces', 'msg'], 'Time'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.state = kwargs.get('state', int())
        self.nav_mode = kwargs.get('nav_mode', int())
        self.curr_lat = kwargs.get('curr_lat', float())
        self.curr_lon = kwargs.get('curr_lon', float())
        self.goal_lat = kwargs.get('goal_lat', float())
        self.goal_lon = kwargs.get('goal_lon', float())
        self.distance_to_goal_m = kwargs.get('distance_to_goal_m', float())
        self.current_yaw_deg = kwargs.get('current_yaw_deg', float())
        self.target_yaw_deg = kwargs.get('target_yaw_deg', float())
        self.heading_error_deg = kwargs.get('heading_error_deg', float())
        self.cone_detected = kwargs.get('cone_detected', bool())
        self.cone_x = kwargs.get('cone_x', float())
        self.cone_y = kwargs.get('cone_y', float())
        self.target_cone_id = kwargs.get('target_cone_id', int())
        self.obstacle_detected = kwargs.get('obstacle_detected', bool())
        self.gps_goal_reached = kwargs.get('gps_goal_reached', bool())
        self.cone_goal_reached = kwargs.get('cone_goal_reached', bool())
        self.autonomous_enabled = kwargs.get('autonomous_enabled', bool())
        from builtin_interfaces.msg import Time
        self.stamp = kwargs.get('stamp', Time())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.state != other.state:
            return False
        if self.nav_mode != other.nav_mode:
            return False
        if self.curr_lat != other.curr_lat:
            return False
        if self.curr_lon != other.curr_lon:
            return False
        if self.goal_lat != other.goal_lat:
            return False
        if self.goal_lon != other.goal_lon:
            return False
        if self.distance_to_goal_m != other.distance_to_goal_m:
            return False
        if self.current_yaw_deg != other.current_yaw_deg:
            return False
        if self.target_yaw_deg != other.target_yaw_deg:
            return False
        if self.heading_error_deg != other.heading_error_deg:
            return False
        if self.cone_detected != other.cone_detected:
            return False
        if self.cone_x != other.cone_x:
            return False
        if self.cone_y != other.cone_y:
            return False
        if self.target_cone_id != other.target_cone_id:
            return False
        if self.obstacle_detected != other.obstacle_detected:
            return False
        if self.gps_goal_reached != other.gps_goal_reached:
            return False
        if self.cone_goal_reached != other.cone_goal_reached:
            return False
        if self.autonomous_enabled != other.autonomous_enabled:
            return False
        if self.stamp != other.stamp:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def state(self):
        """Message field 'state'."""
        return self._state

    @state.setter
    def state(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'state' field must be of type 'int'"
            assert value >= -128 and value < 128, \
                "The 'state' field must be an integer in [-128, 127]"
        self._state = value

    @builtins.property
    def nav_mode(self):
        """Message field 'nav_mode'."""
        return self._nav_mode

    @nav_mode.setter
    def nav_mode(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'nav_mode' field must be of type 'int'"
            assert value >= -128 and value < 128, \
                "The 'nav_mode' field must be an integer in [-128, 127]"
        self._nav_mode = value

    @builtins.property
    def curr_lat(self):
        """Message field 'curr_lat'."""
        return self._curr_lat

    @curr_lat.setter
    def curr_lat(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'curr_lat' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'curr_lat' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._curr_lat = value

    @builtins.property
    def curr_lon(self):
        """Message field 'curr_lon'."""
        return self._curr_lon

    @curr_lon.setter
    def curr_lon(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'curr_lon' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'curr_lon' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._curr_lon = value

    @builtins.property
    def goal_lat(self):
        """Message field 'goal_lat'."""
        return self._goal_lat

    @goal_lat.setter
    def goal_lat(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'goal_lat' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'goal_lat' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._goal_lat = value

    @builtins.property
    def goal_lon(self):
        """Message field 'goal_lon'."""
        return self._goal_lon

    @goal_lon.setter
    def goal_lon(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'goal_lon' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'goal_lon' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._goal_lon = value

    @builtins.property
    def distance_to_goal_m(self):
        """Message field 'distance_to_goal_m'."""
        return self._distance_to_goal_m

    @distance_to_goal_m.setter
    def distance_to_goal_m(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'distance_to_goal_m' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'distance_to_goal_m' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._distance_to_goal_m = value

    @builtins.property
    def current_yaw_deg(self):
        """Message field 'current_yaw_deg'."""
        return self._current_yaw_deg

    @current_yaw_deg.setter
    def current_yaw_deg(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'current_yaw_deg' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'current_yaw_deg' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._current_yaw_deg = value

    @builtins.property
    def target_yaw_deg(self):
        """Message field 'target_yaw_deg'."""
        return self._target_yaw_deg

    @target_yaw_deg.setter
    def target_yaw_deg(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'target_yaw_deg' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'target_yaw_deg' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._target_yaw_deg = value

    @builtins.property
    def heading_error_deg(self):
        """Message field 'heading_error_deg'."""
        return self._heading_error_deg

    @heading_error_deg.setter
    def heading_error_deg(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'heading_error_deg' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'heading_error_deg' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._heading_error_deg = value

    @builtins.property
    def cone_detected(self):
        """Message field 'cone_detected'."""
        return self._cone_detected

    @cone_detected.setter
    def cone_detected(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'cone_detected' field must be of type 'bool'"
        self._cone_detected = value

    @builtins.property
    def cone_x(self):
        """Message field 'cone_x'."""
        return self._cone_x

    @cone_x.setter
    def cone_x(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'cone_x' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'cone_x' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._cone_x = value

    @builtins.property
    def cone_y(self):
        """Message field 'cone_y'."""
        return self._cone_y

    @cone_y.setter
    def cone_y(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'cone_y' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'cone_y' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._cone_y = value

    @builtins.property
    def target_cone_id(self):
        """Message field 'target_cone_id'."""
        return self._target_cone_id

    @target_cone_id.setter
    def target_cone_id(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'target_cone_id' field must be of type 'int'"
            assert value >= -2147483648 and value < 2147483648, \
                "The 'target_cone_id' field must be an integer in [-2147483648, 2147483647]"
        self._target_cone_id = value

    @builtins.property
    def obstacle_detected(self):
        """Message field 'obstacle_detected'."""
        return self._obstacle_detected

    @obstacle_detected.setter
    def obstacle_detected(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'obstacle_detected' field must be of type 'bool'"
        self._obstacle_detected = value

    @builtins.property
    def gps_goal_reached(self):
        """Message field 'gps_goal_reached'."""
        return self._gps_goal_reached

    @gps_goal_reached.setter
    def gps_goal_reached(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'gps_goal_reached' field must be of type 'bool'"
        self._gps_goal_reached = value

    @builtins.property
    def cone_goal_reached(self):
        """Message field 'cone_goal_reached'."""
        return self._cone_goal_reached

    @cone_goal_reached.setter
    def cone_goal_reached(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'cone_goal_reached' field must be of type 'bool'"
        self._cone_goal_reached = value

    @builtins.property
    def autonomous_enabled(self):
        """Message field 'autonomous_enabled'."""
        return self._autonomous_enabled

    @autonomous_enabled.setter
    def autonomous_enabled(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'autonomous_enabled' field must be of type 'bool'"
        self._autonomous_enabled = value

    @builtins.property
    def stamp(self):
        """Message field 'stamp'."""
        return self._stamp

    @stamp.setter
    def stamp(self, value):
        if __debug__:
            from builtin_interfaces.msg import Time
            assert \
                isinstance(value, Time), \
                "The 'stamp' field must be a sub message of type 'Time'"
        self._stamp = value

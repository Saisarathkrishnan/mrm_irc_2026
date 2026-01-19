# generated from rosidl_generator_py/resource/_idl.py.em
# with input from custom_msgs:msg/GuiCommand.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_GuiCommand(type):
    """Metaclass of message 'GuiCommand'."""

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
                'custom_msgs.msg.GuiCommand')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__gui_command
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__gui_command
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__gui_command
            cls._TYPE_SUPPORT = module.type_support_msg__msg__gui_command
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__gui_command

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class GuiCommand(metaclass=Metaclass_GuiCommand):
    """Message class 'GuiCommand'."""

    __slots__ = [
        '_nav_mode',
        '_goal_lat',
        '_goal_lon',
        '_target_cone_id',
        '_set_search_skew',
        '_search_skew',
    ]

    _fields_and_field_types = {
        'nav_mode': 'int8',
        'goal_lat': 'double',
        'goal_lon': 'double',
        'target_cone_id': 'int32',
        'set_search_skew': 'boolean',
        'search_skew': 'int8',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('int8'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('int32'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('int8'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.nav_mode = kwargs.get('nav_mode', int())
        self.goal_lat = kwargs.get('goal_lat', float())
        self.goal_lon = kwargs.get('goal_lon', float())
        self.target_cone_id = kwargs.get('target_cone_id', int())
        self.set_search_skew = kwargs.get('set_search_skew', bool())
        self.search_skew = kwargs.get('search_skew', int())

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
        if self.nav_mode != other.nav_mode:
            return False
        if self.goal_lat != other.goal_lat:
            return False
        if self.goal_lon != other.goal_lon:
            return False
        if self.target_cone_id != other.target_cone_id:
            return False
        if self.set_search_skew != other.set_search_skew:
            return False
        if self.search_skew != other.search_skew:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

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
    def set_search_skew(self):
        """Message field 'set_search_skew'."""
        return self._set_search_skew

    @set_search_skew.setter
    def set_search_skew(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'set_search_skew' field must be of type 'bool'"
        self._set_search_skew = value

    @builtins.property
    def search_skew(self):
        """Message field 'search_skew'."""
        return self._search_skew

    @search_skew.setter
    def search_skew(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'search_skew' field must be of type 'int'"
            assert value >= -128 and value < 128, \
                "The 'search_skew' field must be an integer in [-128, 127]"
        self._search_skew = value

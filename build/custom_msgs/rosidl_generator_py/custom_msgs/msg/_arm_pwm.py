# generated from rosidl_generator_py/resource/_idl.py.em
# with input from custom_msgs:msg/ArmPwm.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_ArmPwm(type):
    """Metaclass of message 'ArmPwm'."""

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
                'custom_msgs.msg.ArmPwm')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__arm_pwm
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__arm_pwm
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__arm_pwm
            cls._TYPE_SUPPORT = module.type_support_msg__msg__arm_pwm
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__arm_pwm

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class ArmPwm(metaclass=Metaclass_ArmPwm):
    """Message class 'ArmPwm'."""

    __slots__ = [
        '_link1',
        '_link2',
        '_swivel',
        '_gripper',
    ]

    _fields_and_field_types = {
        'link1': 'int64',
        'link2': 'int64',
        'swivel': 'int64',
        'gripper': 'int64',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('int64'),  # noqa: E501
        rosidl_parser.definition.BasicType('int64'),  # noqa: E501
        rosidl_parser.definition.BasicType('int64'),  # noqa: E501
        rosidl_parser.definition.BasicType('int64'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.link1 = kwargs.get('link1', int())
        self.link2 = kwargs.get('link2', int())
        self.swivel = kwargs.get('swivel', int())
        self.gripper = kwargs.get('gripper', int())

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
        if self.link1 != other.link1:
            return False
        if self.link2 != other.link2:
            return False
        if self.swivel != other.swivel:
            return False
        if self.gripper != other.gripper:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def link1(self):
        """Message field 'link1'."""
        return self._link1

    @link1.setter
    def link1(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'link1' field must be of type 'int'"
            assert value >= -9223372036854775808 and value < 9223372036854775808, \
                "The 'link1' field must be an integer in [-9223372036854775808, 9223372036854775807]"
        self._link1 = value

    @builtins.property
    def link2(self):
        """Message field 'link2'."""
        return self._link2

    @link2.setter
    def link2(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'link2' field must be of type 'int'"
            assert value >= -9223372036854775808 and value < 9223372036854775808, \
                "The 'link2' field must be an integer in [-9223372036854775808, 9223372036854775807]"
        self._link2 = value

    @builtins.property
    def swivel(self):
        """Message field 'swivel'."""
        return self._swivel

    @swivel.setter
    def swivel(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'swivel' field must be of type 'int'"
            assert value >= -9223372036854775808 and value < 9223372036854775808, \
                "The 'swivel' field must be an integer in [-9223372036854775808, 9223372036854775807]"
        self._swivel = value

    @builtins.property
    def gripper(self):
        """Message field 'gripper'."""
        return self._gripper

    @gripper.setter
    def gripper(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'gripper' field must be of type 'int'"
            assert value >= -9223372036854775808 and value < 9223372036854775808, \
                "The 'gripper' field must be an integer in [-9223372036854775808, 9223372036854775807]"
        self._gripper = value

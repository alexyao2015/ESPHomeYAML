import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_COMMAND
from esphome.components.remote_base import declare_protocol, register_binary_sensor, register_trigger, \
    register_dumper, register_action

CONF_REPEAT = 'repeat'

GiCableData, GiCableBinarySensor, GiCableTrigger, GiCableAction, GiCableDumper = declare_protocol('GiCable')
GiCable_SCHEMA = cv.Schema({
    cv.Required(CONF_COMMAND): cv.hex_uint16_t,
    cv.Required(CONF_REPEAT): cv.boolean,
})

@register_binary_sensor('gicable', GiCableBinarySensor, GiCable_SCHEMA)
def gicable_binary_sensor(var, config):
    cg.add(var.set_data(cg.StructInitializer(
        GiCableData,
        ('command', config[CONF_COMMAND]),
        ('repeat', config[CONF_REPEAT]),
    )))


@register_trigger('gicable', GiCableTrigger, GiCableData)
def gicable_trigger(var, config):
    pass


@register_dumper('gicable', GiCableDumper)
def gicable_dumper(var, config):
    pass

NEC2Data, NEC2BinarySensor, NEC2Trigger, NEC2Action, NEC2Dumper = declare_protocol('NEC2')
NEC2_SCHEMA = cv.Schema({
    cv.Required(CONF_ADDRESS): cv.hex_uint16_t,
    cv.Required(CONF_COMMAND): cv.hex_uint16_t,
    cv.Required(CONF_REPEAT): cv.boolean,
})

@register_binary_sensor('nec2', NEC2BinarySensor, NEC2_SCHEMA)
def nec2_binary_sensor(var, config):
    cg.add(var.set_data(cg.StructInitializer(
        NEC2Data,
        ('address', config[CONF_ADDRESS]),
        ('command', config[CONF_COMMAND]),
        ('repeat', config[CONF_REPEAT]),
    )))


@register_trigger('nec2', NEC2Trigger, NEC2Data)
def nec2_trigger(var, config):
    pass


@register_dumper('nec2', NEC2Dumper)
def nec2_dumper(var, config):
    pass

XMPData, XMPBinarySensor, XMPTrigger, XMPAction, XMPDumper = declare_protocol('XMP')
XMP_SCHEMA = cv.Schema({
    cv.Required(CONF_ADDRESS): cv.hex_uint32_t,
    cv.Required(CONF_COMMAND): cv.hex_uint32_t,
})

@register_binary_sensor('xmp', XMPBinarySensor, XMP_SCHEMA)
def xmp_binary_sensor(var, config):
    cg.add(var.set_data(cg.StructInitializer(
        XMPData,
        ('address', config[CONF_ADDRESS]),
        ('command', config[CONF_COMMAND]),
    )))


@register_trigger('xmp', XMPTrigger, XMPData)
def xmp_trigger(var, config):
    pass


@register_dumper('xmp', XMPDumper)
def xmp_dumper(var, config):
    pass

CONFIG_SCHEMA = cv.Schema({})

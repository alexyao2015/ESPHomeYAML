import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import binary_sensor
from esphome.const import CONF_DATA, CONF_TRIGGER_ID, CONF_NBITS, CONF_ADDRESS, \
    CONF_COMMAND, CONF_CODE, CONF_PULSE_LENGTH, CONF_SYNC, CONF_ZERO, CONF_ONE, CONF_INVERTED, \
    CONF_PROTOCOL, CONF_GROUP, CONF_DEVICE, CONF_STATE, CONF_CHANNEL, CONF_FAMILY, CONF_REPEAT, \
    CONF_WAIT_TIME, CONF_TIMES, CONF_TYPE_ID, CONF_CARRIER_FREQUENCY, CONF_RC_CODE_1, CONF_RC_CODE_2
from esphome.core import coroutine
from esphome.util import Registry, SimpleRegistry

from esphome.components.remote_base import declare_protocol, register_binary_sensor, register_trigger \
    register_dumper, register_action

GiCableData, GiCableBinarySensor, GiCableTrigger, GiCableAction, GiCableDumper = declare_protocol('GiCable')
GiCable_SCHEMA = cv.Schema({
    cv.Required(CONF_COMMAND): cv.hex_uint16_t,
})

@register_binary_sensor('gicable', GiCableBinarySensor, GiCable_SCHEMA)
def gicable_binary_sensor(var, config):
    cg.add(var.set_data(cg.StructInitializer(
        GiCableData,
        ('command', config[CONF_COMMAND]),
    )))


@register_trigger('gicable', GiCableTrigger, GiCableData)
def gicable_trigger(var, config):
    pass

@register_dumper('gicable', GiCableDumper)
def gicable_dumper(var, config):
    pass

@register_action('gicable', GiCableAction, GiCable_SCHEMA)
def gicable_action(var, config, args):
    template_ = yield cg.templatable(config[CONF_ADDRESS], args, cg.uint16)
    cg.add(var.set_address(template_))
    template_ = yield cg.templatable(config[CONF_COMMAND], args, cg.uint16)
    cg.add(var.set_command(template_))

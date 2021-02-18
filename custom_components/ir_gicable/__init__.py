import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_COMMAND
from esphome.components.remote_base import declare_protocol, register_binary_sensor, register_trigger, \
    register_dumper, register_action

CONF_REPEAT = 'repeat'

GiCableData, GiCableBinarySensor, GiCableTrigger, GiCableAction, GiCableDumper = declare_protocol('GiCable')
GiCable_SCHEMA = cv.Schema({
    cv.Required(CONF_COMMAND): cv.hex_uint16_t,
    cv.Required(CONF_REPEAT): cv.uint8_t,
})

@register_binary_sensor('gicable', GiCableBinarySensor, GiCable_SCHEMA)
def gicable_binary_sensor(var, config):
    cg.add(var.set_data(cg.StructInitializer(
        GiCableData,
        ('command', config[CONF_COMMAND]),
        ('repeat', config[CONF_REPEAT])
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

CONFIG_SCHEMA = cv.Schema({})

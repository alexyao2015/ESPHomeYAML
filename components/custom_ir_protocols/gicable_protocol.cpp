#include "gicable_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.gicable";

static const uint32_t HEADER_HIGH_US = 8820;
static const uint32_t HEADER_LOW_US = 4410;
static const uint32_t BIT_HIGH_US = 490;
static const uint32_t BIT_ONE_LOW_US = 4410;
static const uint32_t BIT_ZERO_LOW_US = 2205;
static const uint8_t TOTAL_BITS = 16;
static const uint8_t TOTAL_LENGTH = 36;

GiCableData GiCableProtocol::last_data_received_ = {
    .command = 0,
    .repeat = false,
};
uint32_t GiCableProtocol::last_received_time_ = 0;

void GiCableProtocol::encode(RemoteTransmitData *dst, const GiCableData &data) {}

optional<GiCableData> GiCableProtocol::decode(RemoteReceiveData src) {
  GiCableData data{
      .command = 0,
      .repeat = false,
  };
  
  // Check if full packet
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US) || src.size() != TOTAL_LENGTH) {
    if (millis() - this->last_received_time_ > 150)
      return {};
    // Check if repeat packet
    src.reset();
    if (src.size() == 4 && src.expect_item(HEADER_HIGH_US, BIT_ZERO_LOW_US) && src.expect_mark(BIT_HIGH_US)) {
      this->last_received_time_ = millis();
      return this->last_data_received_;
    } else {
      return {};
    }
  }

  for (uint32_t mask = 1UL << (TOTAL_BITS - 1); mask != 0; mask >>= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.command |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.command &= ~mask;
    } else {
      return {};
    }
  }

  src.expect_mark(BIT_HIGH_US);
  this->last_data_received_ = data;
  this->last_data_received_.repeat = true;
  this->last_received_time_ = millis();
  return data;
}
void GiCableProtocol::dump(const GiCableData &data) {
  ESP_LOGD(TAG, "Received GiCable: command=0x%04X, repeat=%d", data.command, data.repeat);
}

}  // namespace remote_base
}  // namespace esphome
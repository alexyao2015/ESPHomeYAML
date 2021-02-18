// Reimplement ESPHome's NEC protocol to handle repeat signals
// Encoding removed to save space. Use default NEC encoder
#include "nec2_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.nec";

static const uint32_t HEADER_HIGH_US = 9000;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1690;
static const uint32_t BIT_ZERO_LOW_US = 560;
static const uint32_t BIT_LOW_REPEAT = 2205;

NEC2Data NEC2Protocol::last_data_received_ = {
    .address = 0,
    .command = 0,
    .repeat = false,
};
uint32_t NEC2Protocol::last_received_time_ = 0;

void NEC2Protocol::encode(RemoteTransmitData *dst, const NEC2Data &data) {}
optional<NEC2Data> NEC2Protocol::decode(RemoteReceiveData src) {
  NEC2Data data{
      .address = 0,
      .command = 0,
      .repeat = false,
  };
  // Check if full packet
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US)){
    if (millis() - this->last_received_time_ > 150)
      return {};
    // Check if repeat packet
    src.reset();
    if (src.size() == 4 && src.expect_item(HEADER_HIGH_US, BIT_LOW_REPEAT) && src.expect_mark(BIT_HIGH_US)) {
      this->last_received_time_ = millis();
      return this->last_data_received_;
    } else {
      return {};
    }
  }

  for (uint32_t mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.address |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.address &= ~mask;
    } else {
      return {};
    }
  }

  for (uint32_t mask = 1UL << 15; mask != 0; mask >>= 1) {
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
void NEC2Protocol::dump(const NEC2Data &data) {
  ESP_LOGD(TAG, "Received NEC2: address=0x%04X, command=0x%04X, repeat=%d", data.address, data.command, data.repeat);
}

}  // namespace remote_base
}  // namespace esphome
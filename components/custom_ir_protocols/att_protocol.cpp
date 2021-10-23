#include "att_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.att";

static const uint32_t FIRST_HEADER_HIGH_US = 430;
static const uint32_t HEADER_HIGH_US = 180;
static const uint32_t HEADER_LOW_STEP = 260;
static const uint32_t FOOTER_HIGH_US = 180;
// static const uint32_t BIT_FIXED_TOLERANCE = 100;
static const uint32_t BIT_HIGH_US = 180;
static const uint32_t BIT_LOW_STEP = 250;
static const uint8_t BIT_SIZE = 2;
static const uint8_t TOTAL_BITS = 4;
static const uint8_t TOTAL_LENGTH = 36;

void ATTProtocol::encode(RemoteTransmitData *dst, const ATTData &data) {}

optional<ATTData> ATTProtocol::decode(RemoteReceiveData src) {
  ATTData data{
      .command = 0,
      .long_signal = true,
  };

  // Check if full packet
  if (src.size() != TOTAL_LENGTH) {
    return {};
  }
  // Check initial header
  if (!(src.expect_item(FIRST_HEADER_HIGH_US, HEADER_LOW_STEP) && src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP) &&
        src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 2) && src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP) &&
        src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 3) && src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 2) &&
        src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP) && src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP) &&
        src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP))) {
    return {};
  }
  // check if long head
  if (src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 2)) {
    // is long
    data.long_signal = true;
  } else if (src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP)) {
    // is short
    data.long_signal = false;
  } else {
    return {};
  }
  // finish header check
  if (!(src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 2) &&
        src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 1.6) &&
        src.expect_item(HEADER_HIGH_US, HEADER_LOW_STEP * 2.25))) {
    return {};
  }
  for (uint8_t bits = 0; bits < TOTAL_BITS; bits++) {
    if (!src.expect_mark(BIT_HIGH_US)) {
      return {};
    }
    uint8_t value = 1;
    bool found = false;
    for (; value <= 3; value++) {
      // Use static tolerances rather than percentage base tolerances
      if (src.peek() <= 0 && src.peek() >= -(value * BIT_LOW_STEP + BIT_LOW_STEP / 2) &&
          src.peek() <= -(value * BIT_LOW_STEP - BIT_LOW_STEP / 2)) {
        found = true;
        break;
      }
    }
    if (!found) {
      return {};  // Failure.
    }
    value--;
    data.command <<= BIT_SIZE;
    data.command += value;

    src.advance();
  }

  src.expect_mark(FOOTER_HIGH_US);
  return data;
}
void ATTProtocol::dump(const ATTData &data) {
  ESP_LOGD(TAG, "Received ATT: command=0x%02X, long_signal=%d", data.command, data.long_signal);
}

}  // namespace remote_base
}  // namespace esphome

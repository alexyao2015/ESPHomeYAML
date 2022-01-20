#include "att_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.att";

static const uint32_t HEADER_HIGH_US = 400;
static const uint32_t BIT_HIGH_US = 180;
static const uint32_t BIT_ZERO_US = 250;
static const uint32_t BIT_LOW_STEP = 180;
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
  if (!(src.expect_item(HEADER_HIGH_US, BIT_ZERO_US) && src.expect_item(BIT_HIGH_US, BIT_ZERO_US) &&
        src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 2 * BIT_LOW_STEP) && src.expect_item(BIT_HIGH_US, BIT_ZERO_US) &&
        src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 3 * BIT_LOW_STEP) &&
        src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 1 * BIT_LOW_STEP) && src.expect_item(BIT_HIGH_US, BIT_ZERO_US) &&
        src.expect_item(BIT_HIGH_US, BIT_ZERO_US) && src.expect_item(BIT_HIGH_US, BIT_ZERO_US))) {
    return {};
  }
  // check if long head
  if (src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 2 * BIT_LOW_STEP)) {
    // is long
    data.long_signal = true;
  } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_US)) {
    // is short
    data.long_signal = false;
  } else {
    return {};
  }
  // finish header check
  if (!(src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 2 * BIT_LOW_STEP) &&
        src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 1 * BIT_LOW_STEP) &&
        src.expect_item(BIT_HIGH_US, BIT_ZERO_US + 2 * BIT_LOW_STEP))) {
    return {};
  }
  for (uint8_t bits = 0; bits < TOTAL_BITS; bits++) {
    if (!src.expect_mark(BIT_HIGH_US)) {
      return {};
    }
    uint8_t value = 0;
    bool found = false;
    for (; value <= 3; value++) {
      // Use static tolerances rather than percentage base tolerances
      if (src.peek() <= 0 && src.peek() >= -(BIT_ZERO_US + value * BIT_LOW_STEP + BIT_LOW_STEP / 2) &&
          src.peek() <= -(BIT_ZERO_US + value * BIT_LOW_STEP - BIT_LOW_STEP / 2)) {
        found = true;
        break;
      }
    }
    if (!found) {
      return {};  // Failure.
    }
    data.command <<= BIT_SIZE;
    data.command += value;

    src.advance();
  }

  // footer
  src.expect_mark(BIT_HIGH_US);
  return data;
}
void ATTProtocol::dump(const ATTData &data) {
  ESP_LOGD(TAG, "Received ATT: command=0x%02X, long_signal=%d", data.command, data.long_signal);
}

}  // namespace remote_base
}  // namespace esphome

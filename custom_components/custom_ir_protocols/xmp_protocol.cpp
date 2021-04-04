#include "xmp_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.xmp";

static const uint16_t BIT_LOW_ZERO = 786; // Base Value
static const uint16_t BIT_LOW_STEP = 137; // Step increment
static const uint16_t HEADER_HIGH_US = 180;
static const uint16_t FOOTER_HIGH_US = HEADER_HIGH_US;
static const uint16_t FOOTER_LOW_US = 13000;
static const uint8_t TOTAL_BITS = 16;
static const uint8_t TOTAL_LENGTH = 36;
static const uint8_t FIXED_TOLERANCE_VALUE = 100; // Minimum/maximum deviation
static const uint8_t BIT_SIZE = 4;  /// Bits per hex digit

bool validate_checksum(const uint32_t data) {
  uint32_t copy = data;
  uint8_t calculated_checksum = 15;
  uint8_t checksum = copy & 0xF;
  for (uint8_t i = 0; i < 7; i++) {
      copy >>= 4;
      calculated_checksum += copy & 0xF;
  }
  calculated_checksum = ((calculated_checksum & 0xf) ^ 0xf);
  return calculated_checksum == checksum;
}

void XMPProtocol::encode(RemoteTransmitData *dst, const XMPData &data) {}

optional<XMPData> XMPProtocol::decode(RemoteReceiveData src) {
  XMPData data{
      .address = 0,
      .command = 0,
  };
  
  if (src.size() != TOTAL_LENGTH) {
    return {};
  }

  for (uint8_t sections = 0; sections < 2; sections += 1) {
    // first section
    for (uint16_t section_bits = 0; section_bits < TOTAL_BITS / 2; section_bits += 1) {
      if (!(src.peek() <= HEADER_HIGH_US + FIXED_TOLERANCE_VALUE &&
          src.peek() >= HEADER_HIGH_US - FIXED_TOLERANCE_VALUE)) {
        return {};
      }
      src.advance();
      uint8_t value = 0;
      bool found = false;
      for (; value <= 15; value++) {
        // Use static tolerances rather than percentage base tolerances
        if (src.peek() <= 0 &&
            src.peek() >= -(BIT_LOW_ZERO + value * BIT_LOW_STEP + BIT_LOW_STEP / 2) &&
            src.peek() <= -(BIT_LOW_ZERO + value * BIT_LOW_STEP - BIT_LOW_STEP / 2)) {
          found = true;
          break;
        }
      }
      if (!found) {
        return {};  // Failure.
      }
      if (sections == 0) {
        data.address <<= BIT_SIZE;
        data.address += value;
      } else {
        data.command <<= BIT_SIZE;
        data.command += value;
      }
      src.advance();
    }
    // Section Footer
    if (!(src.peek() <= FOOTER_HIGH_US + FIXED_TOLERANCE_VALUE &&
        src.peek() >= FOOTER_HIGH_US - FIXED_TOLERANCE_VALUE)) {
      return {};
    }
    src.advance();
    if (sections == 0) {
      if (!(src.peek_space(FOOTER_LOW_US))) {
        return {};
      }
    } else {
      if (!(src.peek() <= -(FOOTER_LOW_US - FIXED_TOLERANCE_VALUE))) {
        return {};
      }
    }
    src.advance();
  }

  if (!(validate_checksum(data.address) && validate_checksum(data.command))) {
    return {};
  };

  return data;
}
void XMPProtocol::dump(const XMPData &data) {
  ESP_LOGD(TAG, "Received XMP: address=0x%08X, command=0x%08X", data.address, data.command);
}

}  // namespace remote_base
}  // namespace esphome
#include "xmp_protocol.h"
#include "esphome/core/log.h"
#include <inttypes.h>

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.xmp";

static const uint32_t HEADER_HIGH_US = 210;
const uint8_t  kXmpWordSize = 4;  ///< nr. of Bits in a word.
const uint8_t  kXmpMaxWordValue = 15;  // Max word value.
const uint16_t kXmpBaseSpace =     760;  ///< uSeconds
const uint16_t kXmpSpaceStep =     135;  ///< uSeconds
static const uint32_t BIT_ONE_LOW_US = 4410;
static const uint32_t BIT_ZERO_LOW_US = 2205;
static const uint32_t FOOTER_HIGH_US = 210;
static const uint32_t FOOTER_LOW_US = 13000;
static const uint8_t TOTAL_BITS = 16;
static const uint8_t TOTAL_LENGTH = 36;

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
      // if (!src.expect_mark(HEADER_HIGH_US)) return {};
      if (!(src.peek() <= HEADER_HIGH_US + 100 &&
          src.peek() >= HEADER_HIGH_US - 100)) {
        return {};
      }
      src.advance();
      uint8_t value = 0;
      bool found = false;
      for (; value <= 15; value++) {
        // Use static tolerances rather than percentage base tolerances
        if (src.peek() <= 0 &&
            src.peek() >= -(kXmpBaseSpace + value * kXmpSpaceStep + kXmpSpaceStep / 2) &&
            src.peek() <= -(kXmpBaseSpace + value * kXmpSpaceStep - kXmpSpaceStep / 2)) {
          found = true;
          break;
        }
      }
      if (!found) return {};  // Failure.
      if (sections == 0) {
        data.address <<= kXmpWordSize;
        data.address += value;
      } else {
        data.command <<= kXmpWordSize;
        data.command += value;
      }
      src.advance();
    }
    // Section Footer
    if (!(src.peek() <= FOOTER_HIGH_US + 100 &&
        src.peek() >= FOOTER_HIGH_US - 100)) {
      return {};
    }
    src.advance();
    if (sections == 0) {
      if (!(src.peek() >= -(FOOTER_LOW_US + 100) &&
          src.peek() <= -(FOOTER_LOW_US - 100))) {
        return {};
      }
    } else {
      if (!(src.peek() <= -(FOOTER_LOW_US - 100))) {
        return {};
      }
    }
    src.advance();
    // if (!src.expect_item(FOOTER_HIGH_US, FOOTER_LOW_US)) return {};
  }

  return data;
}
void XMPProtocol::dump(const XMPData &data) {
  ESP_LOGD(TAG, "Received XMP: address=0x%08X, command=0x%08X", data.address, data.command);
}

}  // namespace remote_base
}  // namespace esphome
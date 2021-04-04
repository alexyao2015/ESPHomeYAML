#include "xmp_protocol.h"
#include "esphome/core/log.h"
#include <inttypes.h>

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.xmp";

const uint16_t kXmpMark =          210;  ///< uSeconds.
const uint16_t kXmpBits = 64;
const uint16_t kFooter = 2;        // Usual nr. of footer (stop bits) entries.
const uint16_t kStartOffset = 1;   // Usual rawbuf entry to start from.
const uint16_t kXmpBaseSpace =     760;  ///< uSeconds
const uint16_t kXmpSpaceStep =     135;  ///< uSeconds
const uint16_t kXmpFooterSpace = 13000;  ///< uSeconds.
const uint32_t kXmpMessageGap =  80400;  ///< uSeconds.
const uint8_t  kXmpWordSize = 4;  ///< nr. of Bits in a word.
const uint8_t  kXmpMaxWordValue = 15;  // Max word value.
const uint8_t  kXmpSections = 2;  ///< Nr. of Data sections
const uint8_t  kXmpRepeatCode =    0b1000;
const uint8_t  kXmpRepeatCodeAlt = 0b1001;

static const uint32_t HEADER_HIGH_US = 8820;
static const uint32_t HEADER_LOW_US = 4410;
static const uint32_t BIT_HIGH_US = 490;
static const uint32_t BIT_ONE_LOW_US = 4410;
static const uint32_t BIT_ZERO_LOW_US = 2205;
static const uint8_t TOTAL_BITS = 16;
static const uint8_t TOTAL_LENGTH = 36;

XMPData XMPProtocol::last_data_received_ = {
    .command = 0,
    .repeat = false,
};
uint32_t XMPProtocol::last_received_time_ = 0;

uint16_t calcRepeatOffset(const uint16_t nbits) {
  return (nbits < 3 * kNibbleSize) ? 0
                                    : (nbits / kXmpSections) -
                                      (3 * kNibbleSize);
}

bool isRepeat(const uint64_t data, const uint16_t nbits) {
  switch (GETBITS64(data, calcRepeatOffset(nbits), kNibbleSize)) {
    case kXmpRepeatCode:
    case kXmpRepeatCodeAlt:
      return true;
    default:
      return false;
  }
}

void XMPProtocol::encode(RemoteTransmitData *dst, const XMPData &data) {}

optional<XMPData> XMPProtocol::decode(RemoteReceiveData src) {
  XMPData data{
      .command = 0,
      .repeat = false,
  };
  
  if (src.size() < 2 * (kXmpBits / kXmpWordSize) + (kXmpSections * kFooter) + kStartOffset - 1) {
    return {};
  }

  for (uint8_t section = 1; section <= 2; section++) {
    for (uint16_t bits_so_far = 0; bits_so_far < 8; bits_so_far += 1) {
      if (!src.expect_mark(kXmpMark)) return {};
      uint8_t value = 0;
      bool found = false;
      for (; value <= 15; value++) {
        // this is used as the tolarance instead of static percentage kXmpSpaceStep / 2
        if (src.peek() <= 0 &&
            src.peek() >= -(kXmpBaseSpace + value * kXmpSpaceStep + kXmpSpaceStep / 2) &&
            src.peek() <= -(kXmpBaseSpace + value * kXmpSpaceStep - kXmpSpaceStep / 2)) {
          found = true;
          break;
        }
      }
      if (!found) return {};  // Failure.
      data.command <<= kXmpWordSize;
      data.command += value;
      src.advance();
    }
    // Section Footer
    if (!src.expect_mark(kXmpMark)) return {};
    if (section < kXmpSections) {
      if (!src.expect_space(kXmpFooterSpace)) return {};
    }
  }

  this->last_data_received_ = data;
  this->last_data_received_.repeat = true;
  this->last_received_time_ = millis();
  return data;
}
void XMPProtocol::dump(const XMPData &data) {
  ESP_LOGD(TAG, "Received XMP: command=0x%"PRIu64", repeat=%d", data.command, data.repeat);
}

}  // namespace remote_base
}  // namespace esphome
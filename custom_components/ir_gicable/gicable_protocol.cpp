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
static const uint8_t TOTAL_LENGTH = 35;

void GiCableProtocol::encode(RemoteTransmitData *dst, const GiCableData &data) {
  // dst->reserve(68);
  // dst->set_carrier_frequency(38000);

  // dst->item(HEADER_HIGH_US, HEADER_LOW_US);
  // for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
  //   if (data.address & mask)
  //     dst->item(BIT_HIGH_US, BIT_ONE_LOW_US);
  //   else
  //     dst->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  // }

  // for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
  //   if (data.command & mask)
  //     dst->item(BIT_HIGH_US, BIT_ONE_LOW_US);
  //   else
  //     dst->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  // }

  // dst->mark(BIT_HIGH_US);
}
optional<GiCableData> GiCableProtocol::decode(RemoteReceiveData src) {
  GiCableData data{
      .command = 0,
  };
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US) && src.size() != TOTAL_LENGTH)
    return {};

  for (uint32_t mask = 1UL << (TOTAL_BITS - 1); mask != 0; mask >>= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.command |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.command &= ~mask;
    } else {
      return {};
    }
  }

  // for (uint8_t nbits = 0; nbits < TOTAL_BITS; nbits++) {
  //   if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
  //     data.command |= (data.command << 1) | 1;
  //   } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
  //     data.command &= (data.command << 1) | 0;
  //   } else {
  //     return {};
  //   }
  // }

  src.expect_mark(BIT_HIGH_US);
  return data;
}
void GiCableProtocol::dump(const GiCableData &data) {
  ESP_LOGD(TAG, "Received GiCable: command=0x%04X", data.command);
}

}  // namespace remote_base
}  // namespace esphome
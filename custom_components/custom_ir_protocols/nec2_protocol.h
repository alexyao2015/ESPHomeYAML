// Reimplement ESPHome's NEC protocol to handle repeat signals
// Encoding removed to save space. Use default NEC encoder
#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct NEC2Data {
  uint16_t address;
  uint16_t command;
  uint8_t repeat;

  bool operator==(const NEC2Data &rhs) const { return address == rhs.address && command == rhs.command && repeat == rhs.repeat; }
};

class NEC2Protocol : public RemoteProtocol<NEC2Data> {
 public:
  void encode(RemoteTransmitData *dst, const NEC2Data &data) override;
  optional<NEC2Data> decode(RemoteReceiveData src) override;
  void dump(const NEC2Data &data) override;
 protected:
  static NEC2Data last_data_received_;
  static uint32_t last_received_time_;
};

DECLARE_REMOTE_PROTOCOL(NEC2)

}  // namespace remote_base
}  // namespace esphome
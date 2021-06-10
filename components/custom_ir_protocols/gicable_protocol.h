#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct GiCableData {
  uint16_t command;
  bool repeat;

  bool operator==(const GiCableData &rhs) const { return command == rhs.command && repeat == rhs.repeat; }
};

class GiCableProtocol : public RemoteProtocol<GiCableData> {
 public:
  void encode(RemoteTransmitData *dst, const GiCableData &data) override;
  optional<GiCableData> decode(RemoteReceiveData src) override;
  void dump(const GiCableData &data) override;

 protected:
  static GiCableData last_data_received_;
  static uint32_t last_received_time_;
};

DECLARE_REMOTE_PROTOCOL(GiCable)

}  // namespace remote_base
}  // namespace esphome
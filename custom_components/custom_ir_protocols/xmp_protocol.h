#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct XMPData {
  uint64_t command;
  bool repeat;

  bool operator==(const XMPData &rhs) const { return command == rhs.command && repeat == rhs.repeat; }
};

class XMPProtocol : public RemoteProtocol<XMPData> {
 public:
  void encode(RemoteTransmitData *dst, const XMPData &data) override;
  optional<XMPData> decode(RemoteReceiveData src) override;
  void dump(const XMPData &data) override;

 protected:
  static XMPData last_data_received_;
  static uint32_t last_received_time_;
};

DECLARE_REMOTE_PROTOCOL(XMP)

}  // namespace remote_base
}  // namespace esphome
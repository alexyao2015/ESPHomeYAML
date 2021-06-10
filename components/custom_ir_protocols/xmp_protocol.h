#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct XMPData {
  uint32_t address;
  uint32_t command;

  bool operator==(const XMPData &rhs) const { return address == rhs.address && command == rhs.command; }
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
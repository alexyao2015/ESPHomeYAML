#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct ATTData {
  uint32_t command;
  bool long_signal;

  bool operator==(const ATTData &rhs) const { return command == rhs.command && long_signal == rhs.long_signal; }
};

class ATTProtocol : public RemoteProtocol<ATTData> {
 public:
  void encode(RemoteTransmitData *dst, const ATTData &data) override;
  optional<ATTData> decode(RemoteReceiveData src) override;
  void dump(const ATTData &data) override;

 protected:
  static ATTData last_data_received_;
  static uint32_t last_received_time_;
};

DECLARE_REMOTE_PROTOCOL(ATT)

}  // namespace remote_base
}  // namespace esphome
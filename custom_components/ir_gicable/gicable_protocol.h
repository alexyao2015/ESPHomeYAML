#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct GiCableData {
  uint16_t command;

  bool operator==(const GiCableData &rhs) const { return command == rhs.command; }
};

class GiCableProtocol : public RemoteProtocol<GiCableData> {
 public:
  void encode(RemoteTransmitData *dst, const GiCableData &data) override;
  optional<GiCableData> decode(RemoteReceiveData src) override;
  void dump(const GiCableData &data) override;
};

DECLARE_REMOTE_PROTOCOL(GiCable)

template<typename... Ts> class GiCableAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint16_t, command)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    GiCableData data{};
    data.command = this->command_.value(x...);
    GiCableProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
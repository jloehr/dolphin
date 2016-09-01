// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include "WiimoteInput/InputSource.h"

namespace WiimoteInput
{
  class RealDevice : public IInputDevice
  {
  public:
    virtual ~RealDevice() override = default;

    virtual void SetReadCallback(DeviceReadCallback new_callback) override;

  private:
    DeviceReadCallback m_read_callback = nullptr;
    std::mutex m_read_callback_mutex;
  };
}

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

    virtual void SetReadCallback(DeviceReadCallback NewCallback) override;

  private:
    DeviceReadCallback m_ReadCallback = nullptr;
  };
}

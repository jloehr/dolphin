// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/RealWiimotes/RealDevice.h"

namespace WiimoteInput
{
  void RealDevice::SetReadCallback(DeviceReadCallback NewCallback)
  {
    m_ReadCallback = NewCallback;
  }
}
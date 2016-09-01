// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/RealWiimotes/RealDevice.h"

namespace WiimoteInput
{
  void RealDevice::SetReadCallback(DeviceReadCallback new_callback)
  {
    std::lock_guard<std::mutex> lock_guard(m_read_callback_mutex);

    m_read_callback = new_callback;
  }
}
// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/InputSource.h"

namespace WiimoteInput
{
  // Called from ---READ & CPU--- thread
  void InputSource::OnDeviceRead(std::unique_ptr<ReportBuffer> Data)
  {
    // Append Data to ReadQueue
  }

  // Called from ---READ--- thread
  void InputSource::OnDeviceDisconnect()
  {
    m_InputDevice.reset();
  }
}
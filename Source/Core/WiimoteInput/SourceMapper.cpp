// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/SourceMapper.h"

namespace WiimoteInput
{
  void SourceMapper::InterruptChannel(WiimoteID Wiimote, u16 Channel, ReportBuffer Data)
  {
    // Forward to InputSource
  }

  void SourceMapper::ControlChannel(WiimoteID Wiimote, u16 Channel, ReportBuffer Data)
  {
    // Forward to InputSource
  }

  bool SourceMapper::IsConnected(WiimoteID Wiimote)
  {
    // Check Mapping, then Check InputSource
    return false;
  }

  ReportBuffer SourceMapper::PollDataAndUpdate(WiimoteID Wiimote)
  {
    // Update InputSource, then PollData
    return ReportBuffer();
  }

  bool SourceMapper::CheckForConnectionAndUpdate(WiimoteID Wiimote)
  {
    // Update InputSource, then check if it can be conencted to the Emulator
    return false;
  }

  void SourceMapper::SetDisconnected(WiimoteID Wiimote)
  {
    // Forward to InputSource
  }

  const SourceMapping & SourceMapper::GetMapping()
  {
    return InputMapping;
  }

  void SourceMapper::SetMapping(SourceMapping NewMapping)
  {
    // Check if something changed, if so get new number of real wiimotes
    // swirl real wiimotes to match new number
  }

  void SourceMapper::SetMapping(WiimoteID WiimoteSlot, SourceType NewSource)
  {
    // Check if mapping is different
    // Change InputDevice if InputSource
  }
}

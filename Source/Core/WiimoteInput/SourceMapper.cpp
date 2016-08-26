// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/SourceMapper.h"

namespace WiimoteInput
{
  void SourceMapper::InterruptChannel(WiimoteID Wiimote, u16 Channel, std::unique_ptr<ReportBuffer> Data)
  {
    // Forward to InputSource
  }

  void SourceMapper::ControlChannel(WiimoteID Wiimote, u16 Channel, std::unique_ptr<ReportBuffer> Data)
  {
    // Forward to InputSource
  }

  bool SourceMapper::IsConnected(WiimoteID Wiimote) const
  {
    // Check Mapping, then Check InputSource
    return false;
  }

  std::unique_ptr<ReportBuffer> SourceMapper::PollDataAndUpdate(WiimoteID Wiimote)
  {
    // Update InputSource, then PollData
    return std::make_unique<ReportBuffer>();
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

  const SourceMapping& SourceMapper::GetMapping() const
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

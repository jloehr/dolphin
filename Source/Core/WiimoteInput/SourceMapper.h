// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include "WiimoteInput/InputSource.h"
#include "WiimoteInput/RealWiimotes/Scanner.h"
#include "WiimoteInput/WiimoteInput.h"

namespace WiimoteInput
{
  class SourceMapper : public IIO, public ISourceMapping
  {
  public:

    inline IScanner & GetScannerInterface() { return Scanner; };

    // IO Interface
    virtual void InterruptChannel(WiimoteID Wiimote, u16 Channel, ReportBuffer Data) override;
    virtual void ControlChannel(WiimoteID Wiimote, u16 Channel, ReportBuffer Data) override;
    virtual bool IsConnected(WiimoteID Wiimote) override;
    virtual ReportBuffer PollDataAndUpdate(WiimoteID Wiimote) override;
    virtual bool CheckForConnectionAndUpdate(WiimoteID Wiimote) override;
    virtual void SetDisconnected(WiimoteID Wiimote) override;

    // Source Mapping Interface
    virtual const SourceMapping & GetMapping() override;
    virtual void SetMapping(SourceMapping NewMapping) override;
    virtual void SetMapping(WiimoteID WiimoteSlot, SourceType NewSource) override;

    // Called by Scanner in Scanner Thread
    // CB OnNewWiimoteFound
    // CB OnNewBBFound

  private:
    Scanner Scanner;
    // Emulated Wiimote System

    SourceMapping InputMapping;
    std::array<InputSource, MAX_WIIMOTE_DEVICES> InputSources;

    // Unmapped RealWiimotes + BBoard
  };
}

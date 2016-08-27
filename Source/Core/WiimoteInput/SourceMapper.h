// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include <memory>

#include "WiimoteInput/InputSource.h"
#include "WiimoteInput/RealWiimotes/Scanner.h"
#include "WiimoteInput/WiimoteInput.h"

namespace WiimoteInput
{
  // Main Class of the WiimoteInput Module, does the WiimoteID/Channel -> SourceType mapping
  //
  //  SourceMapper -> InputSource -+-> RealDevice
  //                               | 
  //                               +-> EmulatedDevice
  //                               | 
  //                               +-> HybridDevice -> RealDevice
  //                                        | 
  //                                        +-> EmulatedDevice
  //
  class SourceMapper final : public IIO, public ISourceMapping
  {
  public:
    SourceMapper();

    inline IScanner & GetScannerInterface() { return m_Scanner; };

    // IO Interface
    virtual void InterruptChannel(WiimoteID Wiimote, u16 Channel, std::unique_ptr<ReportBuffer> Data) override;
    virtual void ControlChannel(WiimoteID Wiimote, u16 Channel, std::unique_ptr<ReportBuffer> Data) override;
    virtual bool IsConnected(WiimoteID Wiimote) const override;
    virtual std::unique_ptr<ReportBuffer> PollDataAndUpdate(WiimoteID Wiimote) override;
    virtual bool CheckForConnectionAndUpdate(WiimoteID Wiimote) override;
    virtual void SetDisconnected(WiimoteID Wiimote) override;

    // Source Mapping Interface
    virtual const SourceMapping& GetMapping() const override;
    virtual void SetMapping(SourceMapping NewMapping) override;
    virtual void SetMapping(WiimoteID WiimoteSlot, SourceType NewSource) override;

    // Called by Scanner in Scanner Thread
    // CB OnNewWiimoteFound
    // CB OnNewBBFound

  private:
    Scanner m_Scanner;
    // Emulated Wiimote System

    SourceMapping m_InputMapping;
    std::array<std::shared_ptr<InputSource>, NUM_WIIMOTE_DEVICES> m_InputSources;
  };
}

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
    inline IScanner & GetScannerInterface() { return m_scanner; };

    // IO Interface
    virtual void InterruptChannel(WiimoteID wiimote, u16 channel, std::unique_ptr<ReportBuffer> data) override;
    virtual void ControlChannel(WiimoteID wiimote, u16 channel, std::unique_ptr<ReportBuffer> data) override;
    virtual bool IsConnected(WiimoteID wiimote) const override;
    virtual std::unique_ptr<ReportBuffer> PollDataAndUpdate(WiimoteID wiimote) override;
    virtual bool CheckForConnectionAndUpdate(WiimoteID wiimote) override;
    virtual void SetDisconnected(WiimoteID wiimote) override;

    // Source Mapping Interface
    virtual const SourceMapping& GetMapping() const override;
    virtual void SetMapping(SourceMapping new_mapping) override;
    virtual void SetMapping(WiimoteID wiimote, SourceType new_source) override;

    // Called by Scanner in Scanner Thread
    // CB OnNewWiimoteFound
    // CB OnNewBBFound

  private:
    Scanner m_scanner;
    // Emulated Wiimote System

    SourceMapping m_input_mapping;
    std::array<InputSource, NUM_WIIMOTE_DEVICES> m_input_sources;
    // Used for mutual exclusion when the Disconnect Watcher and UI threads
    // try to change the mapping at the same time
    std::mutex m_input_sources_modifier_lock;

  };
}

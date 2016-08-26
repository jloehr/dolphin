// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include <atomic>
#include <memory>
#include <mutex>

#include "WiimoteInput/TypesAndConstants.h"

namespace WiimoteInput
{
  // Callback Interface for the Input Source or Hybrid Decive to recieve newly read data or a disconnect of a RealDevice
  class IParentDevice
  {
  public:
    // Called from ---READ & CPU--- thread
    virtual void OnDeviceRead(ReportBuffer Data) = 0;
    // Called from ---READ--- thread
    virtual void OnDeviceDisconnect() = 0;
  };

  // Interface for the actual input source, i.e. Real Wiimote, Emulated or Hybrid
  class IInputDevice
  {
  public:
    virtual void SetParent(std::shared_ptr<IParentDevice> NewParent) = 0;
    virtual void Write(ReportBuffer Data) = 0;

    // Used for HybridDevice to check if it has a RealDevice
    virtual bool HasInputDevice() { return true; };
  };

  // Class that handles source indepentant common stuff, 
  // like proper data buffering, checking for the OneButton reconnect etc.
  class InputSource : public IParentDevice
  {
  public:
    // Update
    // Write (Control/Interrupt)
    // PollData
    // OnDisconnect

    // HasInputDevice
    // SetInputDevice

    // Parent Device Interface
    // Called from ---READ & CPU--- thread
    virtual void OnDeviceRead(ReportBuffer Data) override;
    // Called from ---READ--- thread
    virtual void OnDeviceDisconnect() override;

  private:
    // Accessed by ---CPU & READ--- thread
    std::shared_ptr<IInputDevice> m_InputDevice;

    // IsDisconnected
    // InputBuffer
  };
}

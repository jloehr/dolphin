// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

namespace WiimoteInput
{
  class InputSource;

  // Interface for the actual input source, i.e. Real Wiimote, Emulated or Hybrid
  class IInputDevice
  {
  public:
    // SetParent(InputSource)
    // Write

    // NeedsInputDevice // Used for HybridDevice to check if it has a RealDevice
  };

  // Object that handles source indepentant common stuff, 
  // like proper data buffering, checking for the OneButton reconnect etc.
  class InputSource
  {
  public:
    // Update
    // Write (Control/Interrupt)
    // PollData
    // OnDisconnect

    // HasInputDevice
    // SetInputDevice

    // InputDeviceReadCB
    // DeviceDisconnectedCB

  private:
    // IsDisconnected
    // InputBuffer
    // InputDevice : RealDevice, EmulatedDevice or HybridDevice
  };
}

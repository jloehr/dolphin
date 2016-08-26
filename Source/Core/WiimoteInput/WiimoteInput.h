// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

// Public Header for all other Modules/Systems that need to interact with the Wiimotes
#pragma once

#include <array>

#include "Common/CommonTypes.h"
#include "WiimoteInput/TypesAndConstants.h"

namespace WiimoteInput
{

  // Interface for the WII IPC HLE that needs to interact with each Wiimote Device
  class IIO
  {
  public:
    virtual void InterruptChannel(WiimoteID Wiimote, u16 Channel, ReportBuffer Data) = 0;
    virtual void ControlChannel(WiimoteID Wiimote, u16 Channel, ReportBuffer Data) = 0;
    // Checks if the actual physical mapped device is currently connected
    virtual bool IsConnected(WiimoteID Wiimote) = 0;
    virtual ReportBuffer PollDataAndUpdate(WiimoteID Wiimote) = 0;
    virtual bool CheckForConnectionAndUpdate(WiimoteID Wiimote) = 0;
    // Called when the Emulator disconnects the virutal Wiimote, used to enable OneButton-reconnect
    virtual void SetDisconnected(WiimoteID Wiimote) = 0;
  };

  // inline IIO& GetIO();

  // Interface for the Core to propagate changes in the Emulator State
  class IEmulatorStateEvents
  {
  public:
    // OnStart
    // OnPause
    // OnResume
    // OnStop
  };

  // inline IEmulatorStateEvents& GetEmulatorStateEvents();

  // Interface to change the mapping of the different Wiimote Devices
  class ISourceMapping
  {
  public:
    virtual const SourceMapping & GetMapping() = 0;
    virtual void SetMapping(SourceMapping NewMapping) = 0;
    virtual void SetMapping(WiimoteID WiimoteSlot, SourceType NewSource) = 0;
  };

  inline ISourceMapping& GetSourceMapping();

  // Interface for the UI regarding Real Wiimote Scanning
  class IScanner
  {
  public:
    virtual bool IsBTAvailable() = 0;
    virtual void SetContinousScanning(bool Enabled) = 0;
    virtual void ScanOnce() = 0;
  };

  // inline IScanner& GetScanner();

  // Interface for the UI to access the Emulated Wiimote Input Settings
  class IEmulated
  {
  public:
    // GetControllerConfig
  };
  // inline IEmulated& GetEmulated();

}
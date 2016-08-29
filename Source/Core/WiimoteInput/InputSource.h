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
  // Interface for the actual input source, i.e. Real Wiimote, Emulated or Hybrid
  class IInputDevice
  {
  public:
    virtual ~IInputDevice() = default;

    using DeviceReadCallback = std::function<void(std::unique_ptr<ReportBuffer>)>;

    virtual void SetReadCallback(DeviceReadCallback NewCallback) = 0;
    virtual void Write(std::unique_ptr<ReportBuffer> Data) = 0;

    // Used for RealDevice if it is still connected and no error occurred
    virtual bool IsGone() = 0;
    // Used for EmulatedDevice as Emulation Pump
    virtual bool Update() = 0;
  };

  // Class that handles source indepentant common stuff, 
  // like proper data buffering, checking for the OneButton reconnect etc.
  class InputSource
  {
  public:
    void InterruptChannel(std::unique_ptr<ReportBuffer> Data);
    void ControlChannel(std::unique_ptr<ReportBuffer> Data);

    void Update();
    std::unique_ptr<ReportBuffer> PollData();

    inline bool IsVirtuallyConnected() const;
    inline bool IsRequestingConnection() const;
    void SetConnected();
    void SetDisconnected();

    bool HasInputDevice() const;
    bool HasInputDeviceAndIsGood() const;
    // Called by SourceMapping changing threads (UI, DisconnectWatcher, etc.)
    std::shared_ptr<IInputDevice> SwapInputDevice(std::shared_ptr<IInputDevice> OtherInputDevice);

    // Called from ---READ & CPU--- thread
    void OnDeviceRead(std::unique_ptr<ReportBuffer> Data);

  private:
    enum class VirtualConnectionStatus
    {
      Disconnected,
      Connected,
      RequestingConnection,
    };

    std::shared_ptr<IInputDevice> m_InputDevice;

    // Accessed by ---READ & CPU--- thread
    std::atomic<VirtualConnectionStatus> m_VirtualConnectionStatus = VirtualConnectionStatus::Disconnected;
    // InputBuffer
  };
}

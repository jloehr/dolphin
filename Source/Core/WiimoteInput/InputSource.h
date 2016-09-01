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
    virtual bool IsGone() const = 0;
    // Used for EmulatedDevice as Emulation Pump
    virtual bool Update() = 0;
  };

  // Class that handles source indepentant common stuff, 
  // like proper data buffering, checking for the OneButton reconnect etc.
  // This class needs to be threadsafe, because the CPU, READ, DISCONNECTWATCHER and UI thread 
  // are interacting with it. Current threadsafety is implemented via the Monitor pattern.
  // The Read Callback is an exception from the Monitor pattern, otherwise
  // there is a risk for a deadlock.
  class InputSource
  {
  public:
    void InterruptChannel(std::unique_ptr<ReportBuffer> data);
    void ControlChannel(std::unique_ptr<ReportBuffer> data);

    void Update();
    std::unique_ptr<ReportBuffer> PollData();

    inline bool IsVirtuallyConnected() const;
    inline bool IsRequestingConnection() const;
    bool SetConnectedIfRequestingConnection();
    void SetDisconnected();

    bool HasInputDeviceAndIsGood() const;

    // Called by SourceMapping changing threads (UI, DISCONNECTWATCHER, etc.)
    std::shared_ptr<IInputDevice> SwapInputDevice(std::shared_ptr<IInputDevice> other_input_device);

    // Called from ---READ & CPU--- thread
    void OnDeviceRead(std::unique_ptr<ReportBuffer> data);

  private:
    enum class VirtualConnectionStatus
    {
      Disconnected,
      Connected,
      RequestingConnection,
    };

    mutable std::mutex m_monitor_mutex;
    std::shared_ptr<IInputDevice> m_input_device;
    std::atomic<VirtualConnectionStatus> m_virtual_connection_status = VirtualConnectionStatus::Disconnected;
    // InputBuffer

  };
}

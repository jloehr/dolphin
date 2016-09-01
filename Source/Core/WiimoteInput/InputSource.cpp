// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/InputSource.h"

namespace WiimoteInput
{
  void InputSource::InterruptChannel(std::unique_ptr<ReportBuffer> Data)
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    // Do HID stuff and call write
  }

  void InputSource::ControlChannel(std::unique_ptr<ReportBuffer> Data)
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    // Do HID stuff and call write
  }

  void InputSource::Update()
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    if (m_input_device)
    {
      m_input_device->Update();
    }
  }

  std::unique_ptr<ReportBuffer> InputSource::PollData()
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    // Pop from the InputBuffer
    return std::unique_ptr<ReportBuffer>();
  }

  inline bool InputSource::IsVirtuallyConnected() const
  {
    return (m_virtual_connection_status.load(std::memory_order_relaxed) == VirtualConnectionStatus::Connected);
  }

  inline bool InputSource::IsRequestingConnection() const
  {
    return (m_virtual_connection_status.load(std::memory_order_relaxed) == VirtualConnectionStatus::RequestingConnection);
  }

  bool InputSource::SetConnectedIfRequestingConnection()
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    // Do the atomic compare and set, so it won't data race with the read callback
    VirtualConnectionStatus expected = VirtualConnectionStatus::RequestingConnection;
    return m_virtual_connection_status.compare_exchange_strong(expected, VirtualConnectionStatus::Connected);
  }

  void InputSource::SetDisconnected()
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    // Deavtivate Continous Reporting for RealWiimotes

    m_virtual_connection_status = VirtualConnectionStatus::Disconnected;
  }

  bool InputSource::HasInputDeviceAndIsGood() const
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    if (!m_input_device)
    {
      return false;
    }

    if (m_input_device->IsGone())
    {
      return false;
    }

    return true;
  }
  
  // Called by SourceMapping changing threads (UI, DisconnectWatcher, etc.)
  std::shared_ptr<IInputDevice> InputSource::SwapInputDevice(std::shared_ptr<IInputDevice> other_input_device)
  {
    std::lock_guard<std::mutex> lock_guard(m_monitor_mutex);

    // This will block until the read thread exits the callback
    m_input_device->SetReadCallback(nullptr);
    // Clear ReadBuffer so we won't forward old reports
    //m_ReportBuffer.clear()
    // Set it to non connected, so the Emulation can reinitialize this Wiimote
    m_virtual_connection_status.store((other_input_device == nullptr) ? VirtualConnectionStatus::Disconnected : VirtualConnectionStatus::RequestingConnection, std::memory_order_release);

    m_input_device.swap(other_input_device);

    if (m_input_device)
    {
      m_input_device->SetReadCallback(std::bind(&InputSource::OnDeviceRead, this, std::placeholders::_1));
    }

    return other_input_device;
  }

  // Called from ---READ & CPU--- thread
  // Do not acquire the monitor lock, otherwise it will deadlock.
  // SwapInputDevice is acquiring the monitor lock, then the read callback lock in RealDevice.
  // To call the callback the RealDevice is acquiring the read callback lock. If this is
  // trying to get the monitor lock, two threads are aquiring two locks in opposite order.
  void InputSource::OnDeviceRead(std::unique_ptr<ReportBuffer> Data)
  {
    if (IsVirtuallyConnected())
    {
      // Append Data to ReadQueue
    }
    else
    {
      // Check if StatusReport, then send ReportMode for Extensions
      // Check if InputReport and check for ButtonPress, then set ConnectionRequested
      // Do an atmoic compare and set, so it won't data race when it is set to connected
    }
  }
}
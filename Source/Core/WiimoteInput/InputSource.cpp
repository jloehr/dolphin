// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/InputSource.h"

namespace WiimoteInput
{
  void InputSource::InterruptChannel(std::unique_ptr<ReportBuffer> Data)
  {
    // Do HID stuff and call write
  }

  void InputSource::ControlChannel(std::unique_ptr<ReportBuffer> Data)
  {
    // Do HID stuff and call write
  }

  void InputSource::Update()
  {
    std::shared_ptr<IInputDevice> ThreadGuardedInputDevice = m_InputDevice;

    if (ThreadGuardedInputDevice)
    {
      ThreadGuardedInputDevice->Update();
    }
  }

  std::unique_ptr<ReportBuffer> InputSource::PollData()
  {
    // Pop from the InputBuffer
    return std::unique_ptr<ReportBuffer>();
  }

  inline bool InputSource::IsVirtuallyConnected() const
  {
    return (m_VirtualConnectionStatus.load() == VirtualConnectionStatus::Connected);
  }

  inline bool InputSource::IsRequestingConnection() const
  {
    return (m_VirtualConnectionStatus.load() == VirtualConnectionStatus::RequestingConnection);
  }

  void InputSource::SetConnected()
  {
    // Reenable Continous Reporting
    m_VirtualConnectionStatus.store(VirtualConnectionStatus::Connected);
  }

  void InputSource::SetDisconnected()
  {
    // Deavtivate Continous Reporting for RealWiimotes
    m_VirtualConnectionStatus.store(VirtualConnectionStatus::Disconnected);
  }

  bool InputSource::HasInputDevice() const
  {
    return (m_InputDevice != nullptr);
  }

  bool InputSource::HasInputDeviceAndIsGood() const
  {
    std::shared_ptr<IInputDevice> ThreadGuardedInputDevice = m_InputDevice;

    if (!ThreadGuardedInputDevice)
    {
      return false;
    }

    if (ThreadGuardedInputDevice->IsGone())
    {
      return false;
    }

    return true;
  }

  // Called by SourceMapping changing threads (UI, DisconnectWatcher, etc.)
  std::shared_ptr<IInputDevice> InputSource::SwapInputDevice(std::shared_ptr<IInputDevice> OtherInputDevice)
  {
    m_InputDevice->SetReadCallback(nullptr);

    OtherInputDevice->SetReadCallback(std::bind(&InputSource::OnDeviceRead, this, std::placeholders::_1));
    m_InputDevice.swap(OtherInputDevice);

    m_VirtualConnectionStatus.store((m_InputDevice == nullptr) ? VirtualConnectionStatus::Disconnected : VirtualConnectionStatus::RequestingConnection);

    return OtherInputDevice;
  }

  // Called from ---READ & CPU--- thread
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
    }
  }
}
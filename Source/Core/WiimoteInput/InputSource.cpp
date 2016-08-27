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
    if (m_InputDevice)
    {
      m_InputDevice->Update();
    }
  }

  std::unique_ptr<ReportBuffer> InputSource::PollData()
  {
    // Pop from the InputBuffer
    return std::unique_ptr<ReportBuffer>();
  }

  inline bool InputSource::IsConnected() const
  {
    return (m_ConnectionStatus.load() == ConnectionStatus::Connected);
  }

  inline bool InputSource::IsRequestingConnection() const
  {
    return (m_ConnectionStatus.load() == ConnectionStatus::RequestingConnection);
  }

  void InputSource::SetConnected()
  {
    // Reenable Continous Reporting
    m_ConnectionStatus.store(ConnectionStatus::Connected);
  }

  void InputSource::SetDisconnected()
  {
    // Deavtivate Continous Reporting for RealWiimotes
    m_ConnectionStatus.store(ConnectionStatus::Disconnected);
  }

  bool InputSource::HasInputDevice()
  {
    if (!m_InputDevice)
    {
      return false;
    }

    // Check if the RealDevice is disconnected
    if (m_InputDevice->IsGone())
    {
      m_InputDevice.reset();
      return false;
    }

    // Check if the HybridDevic has a RealDevice
    if (!m_InputDevice->HasInputDevice())
    {
      return false;
    }

    return true;
  }

  // Called from ---READ & CPU--- thread
  void InputSource::OnDeviceRead(std::unique_ptr<ReportBuffer> Data)
  {
    if (IsConnected())
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
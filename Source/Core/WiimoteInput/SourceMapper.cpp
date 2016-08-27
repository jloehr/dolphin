// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "Common/Assert.h"
#include "WiimoteInput/SourceMapper.h"

namespace WiimoteInput
{
  SourceMapper::SourceMapper()
  {
    // Create InputSources
    for (auto& SharedPointer : m_InputSources)
    {
      SharedPointer = std::make_shared<InputSource>();
    }
  }

  void SourceMapper::InterruptChannel(WiimoteID Wiimote, u16 Channel, std::unique_ptr<ReportBuffer> Data)
  {
    _dbg_assert_msg_(WIIMOTE, Wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_InputMapping[Wiimote] != SourceType::None)
    {
      m_InputSources[Wiimote]->InterruptChannel(std::move(Data));
    }
  }

  void SourceMapper::ControlChannel(WiimoteID Wiimote, u16 Channel, std::unique_ptr<ReportBuffer> Data)
  {
    _dbg_assert_msg_(WIIMOTE, Wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_InputMapping[Wiimote] != SourceType::None)
    {
      m_InputSources[Wiimote]->ControlChannel(std::move(Data));
    }
  }

  bool SourceMapper::IsConnected(WiimoteID Wiimote) const
  {
    _dbg_assert_msg_(WIIMOTE, Wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_InputMapping[Wiimote] != SourceType::None)
    {
      return m_InputSources[Wiimote]->HasInputDevice();
    }

    return false;
  }

  std::unique_ptr<ReportBuffer> SourceMapper::PollDataAndUpdate(WiimoteID Wiimote)
  {
    _dbg_assert_msg_(WIIMOTE, Wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_InputMapping[Wiimote] != SourceType::None)
    {
      std::shared_ptr<InputSource> & InputSource = m_InputSources[Wiimote];
      InputSource->Update();
      return InputSource->PollData();
    }
    
    return nullptr;
  }

  bool SourceMapper::CheckForConnectionAndUpdate(WiimoteID Wiimote)
  {
    _dbg_assert_msg_(WIIMOTE, Wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_InputMapping[Wiimote] != SourceType::None)
    {
      std::shared_ptr<InputSource> & InputSource = m_InputSources[Wiimote];
      InputSource->Update();
      if (InputSource->IsRequestingConnection())
      {
        InputSource->SetConnected();
        return true;
      }
    }

    return false;
  }

  void SourceMapper::SetDisconnected(WiimoteID Wiimote)
  {
    _dbg_assert_msg_(WIIMOTE, Wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_InputMapping[Wiimote] != SourceType::None)
    {
      std::shared_ptr<InputSource> & InputSource = m_InputSources[Wiimote];
      InputSource->SetDisconnected();
    }
  }

  const SourceMapping& SourceMapper::GetMapping() const
  {
    return m_InputMapping;
  }

  void SourceMapper::SetMapping(SourceMapping NewMapping)
  {
    // Check if something changed, if so get new number of real wiimotes
    // swirl real wiimotes to match new number
  }

  void SourceMapper::SetMapping(WiimoteID WiimoteSlot, SourceType NewSource)
  {
    // Check if mapping is different
    // Change InputDevice if InputSource
  }
}

// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <list>

#include "Common/Assert.h"
#include "WiimoteInput/SourceMapper.h"

namespace WiimoteInput
{
  void SourceMapper::InterruptChannel(WiimoteID wiimote, u16 channel, std::unique_ptr<ReportBuffer> data)
  {
    _dbg_assert_msg_(WIIMOTE, wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_input_mapping[wiimote].load(std::memory_order_relaxed) != SourceType::None)
    {
      m_input_sources[wiimote].InterruptChannel(std::move(data));
    }
  }

  void SourceMapper::ControlChannel(WiimoteID wiimote, u16 channel, std::unique_ptr<ReportBuffer> data)
  {
    _dbg_assert_msg_(WIIMOTE, wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_input_mapping[wiimote].load(std::memory_order_relaxed) != SourceType::None)
    {
      m_input_sources[wiimote].ControlChannel(std::move(data));
    }
  }

  bool SourceMapper::IsConnected(WiimoteID wiimote) const
  {
    _dbg_assert_msg_(WIIMOTE, wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_input_mapping[wiimote].load(std::memory_order_relaxed) != SourceType::None)
    {
      const InputSource& input_source = m_input_sources[wiimote];
      return (input_source.HasInputDeviceAndIsGood() && input_source.IsVirtuallyConnected());
    }

    return false;
  }

  std::unique_ptr<ReportBuffer> SourceMapper::PollDataAndUpdate(WiimoteID wiimote)
  {
    _dbg_assert_msg_(WIIMOTE, wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_input_mapping[wiimote].load(std::memory_order_relaxed) != SourceType::None)
    {
      InputSource& input_source = m_input_sources[wiimote];
      input_source.Update();
      return input_source.PollData();
    }
    
    return nullptr;
  }

  bool SourceMapper::CheckForConnectionAndUpdate(WiimoteID wiimote)
  {
    _dbg_assert_msg_(WIIMOTE, wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_input_mapping[wiimote].load(std::memory_order_relaxed) != SourceType::None)
    {
      InputSource& input_source = m_input_sources[wiimote];
      input_source.Update();
      return input_source.SetConnectedIfRequestingConnection();
    }

    return false;
  }

  void SourceMapper::SetDisconnected(WiimoteID wiimote)
  {
    _dbg_assert_msg_(WIIMOTE, wiimote < NUM_WIIMOTE_DEVICES, "WiimoteID = %i >= NumWiimotes = %i");

    if (m_input_mapping[wiimote].load(std::memory_order_relaxed) != SourceType::None)
    {
      m_input_sources[wiimote].SetDisconnected();
    }
  }

  const SourceMapping& SourceMapper::GetMapping() const
  {
    return m_input_mapping;
  }

  void SourceMapper::SetMapping(SourceMapping new_mapping)
  {
    // Check if something changed, if so get new number of real wiimotes
    // swirl real wiimotes to match new number
    std::lock_guard<std::mutex> lock_guard(m_input_sources_modifier_lock);
  
  }

  void SourceMapper::SetMapping(WiimoteID wiimote, SourceType new_source)
  {
    std::lock_guard<std::mutex> lock_guard(m_input_sources_modifier_lock);
    // Check if mapping is different
    // Change InputDevice if InputSource
  }
}

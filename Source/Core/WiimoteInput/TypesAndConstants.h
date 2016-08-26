// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include <array>

#include "Common/CommonTypes.h"

namespace WiimoteInput
{
  constexpr u8 MAX_REPORT_SIZE = 23;
  constexpr u8 MAX_WIIMOTE_DEVICES = 5;

  enum class SourceType
  {
    Real = 0x00
  };

  using WiimoteID = u8;
  using SourceMapping = std::array<SourceType, MAX_WIIMOTE_DEVICES>;
  using ReportBuffer = std::array<u8, MAX_REPORT_SIZE>;
}

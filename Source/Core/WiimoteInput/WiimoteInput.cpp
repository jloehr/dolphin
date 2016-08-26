// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/WiimoteInput.h"
#include "WiimoteInput/SourceMapper.h"

namespace WiimoteInput
{
  SourceMapper g_WiimoteInput;

  IIO & GetIO()
  {
    return g_WiimoteInput;
  }

  ISourceMapping & GetSourceMapping()
  {
    return g_WiimoteInput;
  }

  IScanner & GetScanner()
  {
    return g_WiimoteInput.GetScannerInterface();
  }
}

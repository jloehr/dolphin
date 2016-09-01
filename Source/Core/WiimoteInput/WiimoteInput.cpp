// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "WiimoteInput/WiimoteInput.h"
#include "WiimoteInput/SourceMapper.h"

namespace WiimoteInput
{
  SourceMapper g_Wiimote_Input;

  IIO & GetIO()
  {
    return g_Wiimote_Input;
  }

  ISourceMapping & GetSourceMapping()
  {
    return g_Wiimote_Input;
  }

  IScanner & GetScanner()
  {
    return g_Wiimote_Input.GetScannerInterface();
  }
}

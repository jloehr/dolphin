// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include <vector>

#include "WiimoteInput/WiimoteInput.h"

namespace WiimoteInput
{
  // Interface for all the different platform dependent Scanners
  class IScannerBackend
  {
  public:
    // return List of newly found Wiimotes + BalanceBoard
    virtual void Scan() = 0;
  };

  class Scanner : public IScanner
  {
  public:
    // Ctr (CBs from SourceMapper)

    virtual bool IsBTAvailable() const;
    virtual void SetContinousScanning(bool Enabled);
    virtual void ScanOnce();

  private:
    std::vector<IScannerBackend> m_ScannerBackends;
  };
}

// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.
#pragma once

#include "WiimoteInput/InputSource.h"

namespace WiimoteInput
{
  class RealDevice : public IInputDevice
  {
  public:
    virtual void SetParent(std::shared_ptr<IParentDevice> NewParent) override;

  private:
    std::shared_ptr<IParentDevice> ParentDevice = nullptr;
  };
}

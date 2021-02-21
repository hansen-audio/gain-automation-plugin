//------------------------------------------------------------------------
// Copyright(c) 2021 Hansen Audio.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace ha {
//------------------------------------------------------------------------
static const Steinberg::FUID
    kGainAutomatorProcessorUID(0xCCA38A98, 0x55E653C8, 0x8080AD5A, 0x9F58C064);
static const Steinberg::FUID
    kGainAutomatorControllerUID(0x9AF3E91B, 0x773D537C, 0xB529D773, 0xDF2473B3);

#define GainAutomatorVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace ha

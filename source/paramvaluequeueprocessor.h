// Copyright(c) 2021 Hansen Audio.

#pragma once

#include "paramramp.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

namespace HA {

//------------------------------------------------------------------------
// ParameterChanger
//------------------------------------------------------------------------
class ParamValueQueueProcessor
{
public:
    //--------------------------------------------------------------------
    using ValueType = ParamRamp::ValueType;

    ParamValueQueueProcessor(Steinberg::Vst::IParamValueQueue* queue, ValueType init);
    ParamRamp::ValueType tick();

    //--------------------------------------------------------------------
private:
    int nextSegment();
    void updateRamp();
    void initRamp(int index);

    bool isValidQueue(Steinberg::Vst::IParamValueQueue* queue);
    ParamRamp ramp;
    Steinberg::Vst::IParamValueQueue* queue = nullptr;
    int currSegment                         = 0;
};

//-----------------------------------------------------------------------------
} // namespace HA
// Copyright(c) 2021 Hansen Audio.

#pragma once

#include "paramramp.h"
#include <functional>

namespace HA {
namespace PTB {
//------------------------------------------------------------------------
// ParamValueQueueProcessor
//------------------------------------------------------------------------
class ParamValueQueueProcessor
{
public:
    //--------------------------------------------------------------------
    using ValueType     = ParamRamp::ValueType;
    using mut_ValueType = ParamRamp::mut_ValueType;

    using ParamValueQueue =
        std::function<bool(int /*index*/, int& /*offset*/, mut_ValueType& /*value*/)>;

    ParamValueQueueProcessor(ParamValueQueue queue, ValueType init);
    ValueType tick();
    ValueType getValue() const;

    //--------------------------------------------------------------------
private:
    void updateRamp();
    void initRamp(int index);

    ParamRamp ramp;
    ParamValueQueue queue = nullptr;
    int currSegment       = 0;
    bool moreRamps        = true;
};

//-----------------------------------------------------------------------------
} // namespace PTB
} // namespace HA
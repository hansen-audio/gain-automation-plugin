// Copyright(c) 2021 Hansen Audio.

#include "ha/ptb/paramvaluequeueprocessor.h"
#include <algorithm>

namespace HA {
namespace PTB {

//------------------------------------------------------------------------
// ParamValueQueueProcessor
//------------------------------------------------------------------------
ParamValueQueueProcessor::ParamValueQueueProcessor(ParamValueQueue queue, ValueType init)
: queue(queue)
, ramp({init, init, 0})
{
    initRamp(0);
}

//-----------------------------------------------------------------------------
ParamValueQueueProcessor::ValueType ParamValueQueueProcessor::tick()
{
    if (!moreRamps)
        return ramp.getValue();

    if (ramp.isDone())
    {
        updateRamp();
    }

    return ramp.tick();
}

//-----------------------------------------------------------------------------
ParamValueQueueProcessor::ValueType ParamValueQueueProcessor::getValue() const
{
    return ramp.getValue();
}

//-----------------------------------------------------------------------------
void ParamValueQueueProcessor::updateRamp()
{
    currSegment++;
    initRamp(currSegment);
}

//-----------------------------------------------------------------------------
void ParamValueQueueProcessor::initRamp(int index)
{
    int offset0        = 0;
    mut_ValueType val0 = 0.;
    moreRamps          = queue(index++, offset0, val0);
    if (!moreRamps)
        return;

    int offset1        = 0;
    mut_ValueType val1 = 0.;
    moreRamps          = queue(index, offset1, val1);
    if (!moreRamps)
    {
        ramp = ParamRamp(val0, val0, 0);
        return;
    }

    const int duration = (offset1 - offset0);
    ramp               = ParamRamp(val0, val1, duration);
}

//-----------------------------------------------------------------------------
} // namespace PTB
} // namespace HA
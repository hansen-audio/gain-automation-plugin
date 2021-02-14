// Copyright(c) 2021 Hansen Audio.

#include "paramvaluequeueprocessor.h"
#include <algorithm>

using namespace Steinberg;

namespace HA {

//------------------------------------------------------------------------
// ParamValueQueueProcessor
//------------------------------------------------------------------------

ParamValueQueueProcessor::ParamValueQueueProcessor(Vst::IParamValueQueue* queue, ValueType init)
: queue(queue)
, ramp({init, init, 0})
{
    if (isValidQueue(queue))
        initRamp(0);
}

//-----------------------------------------------------------------------------
ParamRamp::ValueType ParamValueQueueProcessor::tick()
{
    if (!isValidQueue(queue))
        return ramp.getValue();

    if (ramp.isDone())
    {
        updateRamp();
    }

    return ramp.tick();
}

//-----------------------------------------------------------------------------
int ParamValueQueueProcessor::nextSegment()
{
    const int nextSegment = currSegment + 1;
    return std::min(queue->getPointCount(), nextSegment);
}

//-----------------------------------------------------------------------------
void ParamValueQueueProcessor::updateRamp()
{
    currSegment = nextSegment();
    initRamp(currSegment);
}

//-----------------------------------------------------------------------------
void ParamValueQueueProcessor::initRamp(int index)
{
    tresult tres = kResultFalse;

    int offset0          = 0;
    Vst::ParamValue val0 = 0.;
    tres                 = queue->getPoint(index++, offset0, val0);
    if (tres != Steinberg::kResultOk)
        return;

    int offset1          = 0;
    Vst::ParamValue val1 = 0.;
    index                = std::min(index, queue->getPointCount() - 1);
    tres                 = queue->getPoint(index, offset1, val1);
    if (tres != Steinberg::kResultOk)
        return;

    const int duration = (offset1 - offset0); // - 1;
    ramp               = ParamRamp(val0, val1, duration);
}

//-----------------------------------------------------------------------------
bool ParamValueQueueProcessor::isValidQueue(Vst::IParamValueQueue* queue)
{
    if (!queue)
        return false;

    if (queue->getPointCount() == 0)
        return false;

    return true;
}

//-----------------------------------------------------------------------------
} // namespace HA
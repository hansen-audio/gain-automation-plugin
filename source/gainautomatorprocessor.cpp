//------------------------------------------------------------------------
// Copyright(c) 2021 Hansen Audio.
//------------------------------------------------------------------------

#include "gainautomatorprocessor.h"
#include "gainautomatorcids.h"
#include "gainautomatorparamids.h"
#include "ha/ptb/paramvaluequeueprocessor.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <algorithm>
#include <cmath>

using namespace Steinberg;

namespace HA {
namespace {

//------------------------------------------------------------------------
using QueueProcessor = PTB::ParamValueQueueProcessor;
QueueProcessor createPVQP(Vst::IParamValueQueue* queue, float initValue)
{
    const auto pvqp = [queue](int index, int& offset, QueueProcessor::mut_ValueType& value) {
        if (!queue)
            return false;

        if (index < queue->getPointCount())
        {
            Vst::ParamValue tmpValue = 0.;
            if (queue->getPoint(index, offset, tmpValue) != kResultOk)
                return false;

            value = tmpValue;
            return true;
        }

        return false;
    };
    return QueueProcessor(pvqp, initValue);
}

//------------------------------------------------------------------------
Vst::IParamValueQueue* findParamValueQueue(Vst::ParamID id, Vst::IParameterChanges* paramChanges)
{
    Vst::IParamValueQueue* queue = nullptr;
    if (!paramChanges)
        return queue;

    const int32 numParamsChanges = paramChanges->getParameterCount();
    for (int32 index = 0; index < numParamsChanges; index++)
    {
        if (auto* paramQueue = paramChanges->getParameterData(index))
        {
            if (paramQueue->getParameterId() == id)
            {
                queue = paramQueue;
                break;
            }
        }
    }

    return queue;
}

//------------------------------------------------------------------------
} // namespace

//------------------------------------------------------------------------
// GainAutomatorProcessor
//------------------------------------------------------------------------
GainAutomatorProcessor::GainAutomatorProcessor()
{
    setControllerClass(kGainAutomatorControllerUID);
}

//------------------------------------------------------------------------
GainAutomatorProcessor::~GainAutomatorProcessor() {}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::initialize(FUnknown* context)
{
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk)
    {
        return result;
    }

    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::terminate()
{
    return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::setActive(TBool state)
{
    return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::process(Vst::ProcessData& data)
{
    auto* gainQueue = findParamValueQueue(kParamGainId, data.inputParameterChanges);
    PTB::ParamValueQueueProcessor gainProc = createPVQP(gainQueue, gainValue);

    if (!data.outputs || !data.inputs)
        return kResultOk;

    for (int i = 0; i < data.numSamples; ++i)
    {
        if (data.outputs[0].channelBuffers32[0] && data.inputs[0].channelBuffers32[0])
            data.outputs[0].channelBuffers32[0][i] =
                data.inputs[0].channelBuffers32[0][i] * gainValue;

        if (data.outputs[0].channelBuffers32[1] && data.inputs[0].channelBuffers32[1])
            data.outputs[0].channelBuffers32[1][i] =
                data.inputs[0].channelBuffers32[1][i] * gainValue;

        gainValue = gainProc.tick();
    }

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::setupProcessing(Vst::ProcessSetup& newSetup)
{
    return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
    if (symbolicSampleSize == Vst::kSample32)
        return kResultTrue;

    return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::setState(IBStream* state)
{
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::getState(IBStream* state)
{
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace HA

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
    Vst::IParamValueQueue* gainQueue = nullptr;
    if (data.inputParameterChanges)
    {
        const int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (int32 index = 0; index < numParamsChanged; index++)
        {
            if (auto* paramQueue = data.inputParameterChanges->getParameterData(index))
            {
                if (paramQueue->getParameterId() == kParamGainId)
                {
                    gainQueue = paramQueue;
                }
            }
        }
    }

    PTB::ParamValueQueueProcessor gainProc(
        [gainQueue](int index, int& offset, PTB::ParamValueQueueProcessor::mut_ValueType& value) {
            if (!gainQueue)
                return false;

            if (index < gainQueue->getPointCount())
            {
                Vst::ParamValue tmpValue = 0.;
                if (gainQueue->getPoint(index, offset, tmpValue) != kResultOk)
                    return false;

                value = tmpValue;
                return true;
            }

            return false;
        },
        gainValue);

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

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
    //--- set the wanted controller for our processor
    setControllerClass(kGainAutomatorControllerUID);
}

//------------------------------------------------------------------------
GainAutomatorProcessor::~GainAutomatorProcessor() {}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::initialize(FUnknown* context)
{
    // Here the Plug-in will be instanciated

    //---always initialize the parent-------
    tresult result = AudioEffect::initialize(context);
    // if everything Ok, continue
    if (result != kResultOk)
    {
        return result;
    }

    //--- create Audio IO ------
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

    /* If you don't need an event bus, you can remove the next line */
    addEventInput(STR16("Event In"), 1);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::terminate()
{
    // Here the Plug-in will be de-instanciated, last possibility to remove some memory!

    //---do not forget to call parent ------
    return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::setActive(TBool state)
{
    //--- called when the Plug-in is enable/disable (On/Off) -----
    return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::process(Vst::ProcessData& data)
{
    //--- First : Read inputs parameter changes-----------

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
        [&](int index, int& offset, PTB::ParamValueQueueProcessor::mut_ValueType& value) {
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

    for (int i = 0; i < data.numSamples; ++i)
    {
        data.outputs[0].channelBuffers32[0][i] = data.inputs[0].channelBuffers32[0][i] * gainValue;
        data.outputs[0].channelBuffers32[1][i] = data.inputs[0].channelBuffers32[1][i] * gainValue;
        gainValue                              = gainProc.tick();
    }

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::setupProcessing(Vst::ProcessSetup& newSetup)
{
    //--- called before any processing ----
    return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
    // by default kSample32 is supported
    if (symbolicSampleSize == Vst::kSample32)
        return kResultTrue;

    // disable the following comment if your processing support kSample64
    /* if (symbolicSampleSize == Vst::kSample64)
        return kResultTrue; */

    return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::setState(IBStream* state)
{
    // called when we load a preset, the model has to be reloaded
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorProcessor::getState(IBStream* state)
{
    // here we need to save the model
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace HA

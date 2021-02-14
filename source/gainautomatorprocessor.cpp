//------------------------------------------------------------------------
// Copyright(c) 2021 Hansen Audio.
//------------------------------------------------------------------------

#include "gainautomatorprocessor.h"
#include "gainautomatorcids.h"
#include "gainautomatorparamids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <algorithm>
#include <cmath>

using namespace Steinberg;

namespace HA {

//------------------------------------------------------------------------
template <typename T>
T dezip(T x, T delta, T dst)
{
    const T kDiff = dst - x;
    if (kDiff > delta)
        x += delta;
    else if (kDiff < -delta)
        x -= delta;
    else
        x = dst;

    return x;
}

//------------------------------------------------------------------------
template <typename T>
T calcDelta(T cur, T dst, int n)
{
    return fabs(dst - cur) / static_cast<T>(n);
}

//------------------------------------------------------------------------
//  ParamRamp
//------------------------------------------------------------------------
class ParamRamp
{
public:
    //------------------------------------------------------------------------
    using ValueType     = const float;
    using mut_ValueType = float;

    ParamRamp(ValueType src, ValueType dst, int _numSamples)
    : x(src)
    , dst(dst)
    , numSamples(_numSamples)
    {
        numSamples = numSamples == 0 ? 1 : numSamples;
        delta      = calcDelta(x, dst, numSamples);
    }

    ValueType tick()
    {
        x = dezip(x, delta, dst);
        return x;
    }

    ValueType getValue() const { return x; };

    bool isDone() const { return x == dst; }

    //------------------------------------------------------------------------
private:
    mut_ValueType dst   = 0.;
    mut_ValueType delta = 0.1;
    mut_ValueType x     = 0.;
    int numSamples      = 32;
};

//------------------------------------------------------------------------
// ParameterChanger
//------------------------------------------------------------------------
class ParamValueQueueProcessor
{
public:
    //--------------------------------------------------------------------
    using ValueType = ParamRamp::ValueType;

    ParamValueQueueProcessor(Vst::IParamValueQueue* queue, ValueType init)
    : queue(queue)
    , ramp({init, init, 0})
    {
        if (isValidQueue(queue))
            initRamp(0);
    }

    ParamRamp::ValueType tick()
    {
        if (!isValidQueue(queue))
            return ramp.getValue();

        if (ramp.isDone())
        {
            updateRamp();
        }

        return ramp.tick();
    }

    //--------------------------------------------------------------------
private:
    int nextSegment()
    {
        const int nextSegment = currSegment + 1;
        return std::min(queue->getPointCount(), nextSegment);
    }

    void updateRamp()
    {
        currSegment = nextSegment();
        initRamp(currSegment);
    }

    void initRamp(int index)
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

    bool isValidQueue(Vst::IParamValueQueue* queue)
    {
        if (!queue)
            return false;

        if (queue->getPointCount() == 0)
            return false;

        return true;
    }

    ParamRamp ramp;
    Vst::IParamValueQueue* queue = nullptr;
    int currSegment              = 0;
};

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

    if (!gainQueue)
        return kResultOk; // TODO: Pass through buffers.

    ParamValueQueueProcessor gainProc(gainQueue, gainValue);
    int32 samples = data.numSamples;
    while (samples-- > 0)
    {
        gainValue = gainProc.tick();
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

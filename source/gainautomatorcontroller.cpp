//------------------------------------------------------------------------
// Copyright(c) 2021 Hansen Audio.
//------------------------------------------------------------------------

#include "gainautomatorcontroller.h"
#include "gainautomatorcids.h"
#include "gainautomatorparamids.h"
#include "pluginterfaces/base/ustring.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace HA {

//------------------------------------------------------------------------
// GainParameter
//------------------------------------------------------------------------
class GainParameter : public Vst::Parameter
{
public:
    GainParameter(int32 flags, int32 id);

    void toString(Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
    bool fromString(const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
// GainParameter Implementation
//------------------------------------------------------------------------
GainParameter::GainParameter(int32 flags, int32 id)
{
    Steinberg::UString(info.title, USTRINGSIZE(info.title)).assign(USTRING("Gain"));
    Steinberg::UString(info.units, USTRINGSIZE(info.units)).assign(USTRING("dB"));

    info.flags                  = flags;
    info.id                     = id;
    info.stepCount              = 0;
    info.defaultNormalizedValue = 1.;
    info.unitId                 = Vst::kRootUnitId;

    setNormalized(1.f);
}

//------------------------------------------------------------------------
void GainParameter::toString(Vst::ParamValue normValue, Vst::String128 string) const
{
    char text[32];
    if (normValue <= 0.)
    {
        strcpy(text, "-inf");
    }
    else
    {
        sprintf(text, "%.2f", 20 * log10f(static_cast<float>(normValue)));
    }

    Steinberg::UString(string, 128).fromAscii(text);
}

//------------------------------------------------------------------------
bool GainParameter::fromString(const Vst::TChar* string, Vst::ParamValue& normValue) const
{
    String wrapper((Vst::TChar*)string); // don't know buffer size here!
    double physValue = 0.0;
    if (wrapper.scanFloat(physValue))
    {
        // Only allow values =< 0dB
        physValue = physValue > 0. ? 0. : physValue;
        normValue = expf(logf(10.f) * static_cast<float>(physValue) / 20.f);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
// GainAutomatorController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::initialize(FUnknown* context)
{
    tresult result = EditControllerEx1::initialize(context);
    if (result != kResultOk)
        return result;

    parameters.addParameter(new GainParameter(Vst::ParameterInfo::kCanAutomate, kParamGainId));

    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::terminate()
{
    return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::setComponentState(IBStream* state)
{
    if (!state)
        return kResultFalse;

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::setState(IBStream* state)
{
    return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::getState(IBStream* state)
{
    return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API GainAutomatorController::createView(FIDString name)
{
    if (FIDStringsEqual(name, Vst::ViewType::kEditor))
    {
        auto* view = new VSTGUI::VST3Editor(this, "view", "gainautomatoreditor.uidesc");
        return view;
    }
    return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::setParamNormalized(Vst::ParamID tag,
                                                               Vst::ParamValue value)
{
    tresult result = EditControllerEx1::setParamNormalized(tag, value);
    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::getParamStringByValue(Vst::ParamID tag,
                                                                  Vst::ParamValue valueNormalized,
                                                                  Vst::String128 string)
{
    return EditControllerEx1::getParamStringByValue(tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API GainAutomatorController::getParamValueByString(Vst::ParamID tag,
                                                                  Vst::TChar* string,
                                                                  Vst::ParamValue& valueNormalized)
{
    return EditControllerEx1::getParamValueByString(tag, string, valueNormalized);
}

//------------------------------------------------------------------------
} // namespace HA

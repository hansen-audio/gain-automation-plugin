//------------------------------------------------------------------------
// Copyright(c) 2021 Hansen Audio.
//------------------------------------------------------------------------

#include "gain_automator_controller.h"
#include "gain_automator_cids.h"
#include "gain_automator_param_ids.h"
#include "ha/param_tool_box/convert/dezibel.h"
#include "pluginterfaces/base/ustring.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace ha {

//------------------------------------------------------------------------
using fdezibel = ptb::convert::dezibel<float>;
static const fdezibel dB_converter(-96, 0);

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
    const auto physical    = dB_converter.to_physical(normValue);
    const auto physicalStr = dB_converter.to_string(physical, [](fdezibel::value_type physical) {
        return physical < fdezibel::value_type(-10.) ? 1 : 2;
    });
    Steinberg::UString(string, 128).fromAscii(physicalStr.data());
}

//------------------------------------------------------------------------
bool GainParameter::fromString(const Vst::TChar* string, Vst::ParamValue& normValue) const
{
    const auto utf8_str       = VST3::StringConvert::convert(string);
    const auto physical_val   = dB_converter.from_string(utf8_str);
    const auto normalized_val = dB_converter.to_normalized(physical_val);
    normValue                 = normalized_val;
    return true;
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
        auto* view = new VSTGUI::VST3Editor(this, "view", "gain_automator_editor.uidesc");
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
} // namespace ha

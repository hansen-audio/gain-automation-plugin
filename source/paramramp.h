// Copyright(c) 2021 Hansen Audio.

#pragma one

namespace HA {

//------------------------------------------------------------------------
//  ParamRamp
//------------------------------------------------------------------------
class ParamRamp
{
public:
    //------------------------------------------------------------------------
    using ValueType     = const float;
    using mut_ValueType = float;

    ParamRamp(ValueType src, ValueType dst, int _numSamples);
    ValueType tick();
    ValueType getValue() const { return x; };
    bool isDone() const;

    //------------------------------------------------------------------------
private:
    mut_ValueType dst   = 0.;
    mut_ValueType delta = 0.1;
    mut_ValueType x     = 0.;
    int numSamples      = 32;
};

//-----------------------------------------------------------------------------
} // namespace HA
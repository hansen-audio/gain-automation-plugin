// Copyright(c) 2021 Hansen Audio.

#include "ha/ptb/paramramp.h"
#include <math.h>

namespace HA {
namespace {

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
} // namespace

namespace PTB {
//-----------------------------------------------------------------------------
// ParamRamp
//-----------------------------------------------------------------------------
ParamRamp::ParamRamp(ValueType src, ValueType dst, int _numSamples)
: x(src)
, dst(dst)
, numSamples(_numSamples)
{
    numSamples = numSamples == 0 ? 1 : numSamples;
    delta      = calcDelta(x, dst, numSamples);
}

//-----------------------------------------------------------------------------
ParamRamp::ValueType ParamRamp::tick()
{
    x = dezip(x, delta, dst);
    return x;
}

//-----------------------------------------------------------------------------
bool ParamRamp::isDone() const
{
    return x == dst;
}

//-----------------------------------------------------------------------------
} // namespace PTB
} // namespace HA
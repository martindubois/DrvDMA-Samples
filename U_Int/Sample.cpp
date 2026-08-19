
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_Int/Sample.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "Sample.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Sample::Sample() : mBeforeTrig(0), mAfterTrig(0), mOnInterrupt(0), mInterrupts(0), mIteration(0) {}

double Sample::GetTrig() const
{
    assert(mAfterTrig >= mBeforeTrig);

    uint64_t lTrig = mAfterTrig - mBeforeTrig;

    double lResult_us = static_cast<double>(lTrig);

    lResult_us /= gFrequency;
    lResult_us *= 1000000;

    return lResult_us;
}

double Sample::GetUser() const
{
    assert(mBeforeTrig <= mOnInterrupt);

    uint64_t lUser = mOnInterrupt - mBeforeTrig;

    double lResult_us = static_cast<double>(lUser);

    lResult_us /= gFrequency;
    lResult_us *= 1000000;

    return lResult_us;
}

bool Sample::IsValid() const
{
    return (0 < mBeforeTrig)
        && (mBeforeTrig <= mAfterTrig)
        && (mBeforeTrig <= mOnInterrupt)
        && (0 != mInterrupts);
}

void Sample::BeforeTrig(unsigned int aIteration)
{
    auto lRetB = QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mBeforeTrig));
    assert(lRetB);

    mIteration = aIteration;
}

void Sample::AfterTrig()
{
    auto lRetB = QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mAfterTrig));
    assert(lRetB);
}

void Sample::OnInterrupt(uint64_t aInterrupts)
{
    auto lRetB = QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mOnInterrupt));
    assert(lRetB);

    mInterrupts = aInterrupts;
}

std::ostream& operator << (std::ostream& aOut, const Sample& aIn)
{
    aOut << aIn.mBeforeTrig << ";" << aIn.mAfterTrig << ";" << aIn.mOnInterrupt << ";" << aIn.mInterrupts << ";" << aIn.mIteration;

    return aOut;
}

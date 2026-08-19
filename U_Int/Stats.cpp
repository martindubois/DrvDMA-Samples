
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_Int/Stats.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "Stats.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Stats::Stats() : mN(0), mSum(0.0), mSum2(0.0) {}

double Stats::GetAverage() const
{
    double lResult = 0.0;

    if (0 < mN)
    {
        lResult = mSum;
        lResult /= mN;
    }

    return lResult;
}

unsigned int Stats::GetCount() const { return mN; }

double Stats::GetMax() const
{
    double lResult = 0.0;

    if (0 < mN)
    {
        lResult = mMax;
    }

    return lResult;
}

double Stats::GetMin() const
{
    double lResult = 0.0;

    if (0 < mN)
    {
        lResult = mMin;
    }

    return lResult;
}

double Stats::GetStdDev() const
{
    assert(0.0 <= mSum2);

    double lResult = 0.0;
    
    if (2 <= mN)
    {
        lResult = mSum2;
        lResult -= mSum * mSum / mN;
        lResult /= mN - 1;

        assert(0 < lResult);

        lResult = sqrt(lResult);
    }

    return lResult;
}

void Stats::AddSample(double aValue)
{
    if (0 == mN)
    {
        mMax = aValue;
        mMin = aValue;
    }
    else
    {
        if (mMax < aValue)
        {
            mMax = aValue;
        }

        if (mMin > aValue)
        {
            mMin = aValue;
        }
    }

    mN++;
    mSum += aValue;
    mSum2 += aValue * aValue;
}

std::ostream& operator << (std::ostream& aOut, const Stats& aIn)
{
    aOut << aIn.GetMin() << " us < " << aIn.GetAverage() << " us < " << aIn.GetMax() << " us ; " << aIn.GetStdDev() << " us ( " << aIn.GetCount() << " )";

    return aOut;
}

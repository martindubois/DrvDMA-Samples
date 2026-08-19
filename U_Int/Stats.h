
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_Int/Stats.h

#pragma once

class Stats
{

public:

    Stats();

    double GetAverage() const;
    unsigned int GetCount() const;
    double GetMax() const;
    double GetMin() const;
    double GetStdDev() const;

    void AddSample(double aValue);

private:

    unsigned int mN;

    double mMax;
    double mMin;
    double mSum;
    double mSum2;

};

std::ostream& operator << (std::ostream& aOut, const Stats& aIn);

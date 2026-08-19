
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_Int/Sample.h

#pragma once

class Sample
{

public:

    Sample();

    double GetTrig() const;
    double GetUser() const;

    bool IsValid() const;

    void BeforeTrig(unsigned int aIteration);
    void AfterTrig();
    void OnInterrupt(uint64_t aInterrupts);

    friend std::ostream& operator << (std::ostream& aOut, const Sample& aIn);

private:

    uint64_t mBeforeTrig;
    uint64_t mAfterTrig;
    uint64_t mOnInterrupt;

    uint64_t mInterrupts;

    unsigned int mIteration;

};

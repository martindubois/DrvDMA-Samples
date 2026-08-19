
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_Int/U_Int.cpp

// Intel 82576EB Gigabit Ethernet Controller Datasheet
// https://www.intel.com/content/dam/www/public/us/en/documents/datasheets/82576eg-gbe-datasheet.pdf

#include "Component.h"

// ===== DrvDMA =============================================================
#include <DrvDMA_U.h>

// ===== Local ==============================================================
#include "Sample.h"
#include "Stats.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static constexpr uint64_t INT_MASK = 1;

static constexpr unsigned int ITERATION = 1000;

static constexpr unsigned int PERIOD_ms = 100;

static constexpr auto SIMULATE = false;

static constexpr auto TRIG_MAX_us = 20;

// Variables
// //////////////////////////////////////////////////////////////////////////

uint64_t gFrequency;

static Sample sSamples[ITERATION];

static volatile unsigned int sInterruptCount = 0;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry point ========================================================

static void OnInterrupt(void* aContext, uint64_t aInterrupts);

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define DEVICE_INDEX (0)

int main()
{
    int lResult = __LINE__;

    Stats lTrig;
    Stats lUser;

    auto lRetB = QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&gFrequency));
    assert(lRetB);

    std::cout << "Performance counter frequency : " << gFrequency << " Hz" << std::endl;

    auto lDD = DrvDMA::Create();
    if (nullptr == lDD)
    {
        std::cout << "ERROR  DrvDMA::Create  failed" << std::endl;
        return __LINE__;
    }

    auto lRet = lDD->Connect(DEVICE_INDEX);
    if (DrvDMA_OK != lRet)
    {
        std::cout << "ERROR  DrvDMA::Connect  failed - " << lRet << std::endl;
        lResult = __LINE__;
        goto End0;
    }

    std::cout << "Connected" << std::endl;

    volatile void* lBAR0;
    
    lBAR0 = lDD->Memory_GetAddress(0);
    if (nullptr == lBAR0)
    {
        std::cout << "ERROR  DrvDMA::Memory_GetAddress  failed" << std::endl;
        lResult = __LINE__;
        goto End1;
    }

    unsigned int lBAR0_Size_byte;
    
    lBAR0_Size_byte = lDD->Memory_GetSize(0);
    if ((128 * 1024) > lBAR0_Size_byte)
    {
        std::cout << "ERROR  The BAR0 is smaller than expected (" << lBAR0_Size_byte << " bytes)" << std::endl;
        lResult = __LINE__;
        goto End1;
    }

    lRet = lDD->Interrupt_Register(INT_MASK, 0, OnInterrupt, nullptr);
    if (DrvDMA_OK != lRet)
    {
        std::cout << "ERROR  DrvDMA::Interrupt_Register  failed - " << lRet << std::endl;
        lResult = __LINE__;
        goto End1;
    }

    // TODO  Configure the Intel chip
    volatile uint32_t* lReg;

    lReg = reinterpret_cast<volatile uint32_t*>(lBAR0);

    lReg[0x1508 / sizeof(uint32_t)] = INT_MASK;

    for (unsigned int i = 0; i < ITERATION; i++)
    {
        std::cout << i << " " << sInterruptCount << "\r";

        auto lDummy = lReg[0x1500 / sizeof(uint32_t)];

        auto lSample = sSamples + sInterruptCount;

        lSample->BeforeTrig(i);

        if (SIMULATE)
        {
            lRet = lDD->Interrupt_Simulate(INT_MASK);
        }
        else
        {
            lReg[0x1504 / sizeof(uint32_t)] = INT_MASK;
        }

        lSample->AfterTrig();

        Sleep(PERIOD_ms);
    }

    std::cout << std::endl;

    lRet = lDD->Interrupt_Unregister();
    assert(DrvDMA_OK == lRet);

    if (ITERATION > sInterruptCount)
    {
        std::cout << "WARNING  " << (ITERATION - sInterruptCount) << " interrupt lost" << std::endl;
    }

    for (unsigned int i = 0; i < sInterruptCount; i++)
    {
        auto lSample = sSamples + i;

        std::cout << *lSample;

        if (lSample->IsValid())
        {
            auto lTrig_us = sSamples[i].GetTrig();
            auto lUser_us = sSamples[i].GetUser();

            std::cout << ";" << lTrig_us << ";" << lUser_us;

            lTrig.AddSample(lTrig_us);

            if (TRIG_MAX_us >= lTrig_us)
            {
                lUser.AddSample(lUser_us);

                std::cout << ";Used\n";
            }
            else
            {
                std::cout << ";Ignored\n";
            }
        }
        else
        {
            std::cout << ";;;Invalid\n";
        }
    }

    std::cout << "Trigger : " << lTrig << std::endl;
    std::cout << "User    : " << lUser << std::endl;

    lResult = 0;

End1:
    lRet = lDD->Disconnect();
    assert(DrvDMA_OK == lRet);

    std::cout << "Disconnected" << std::endl;

End0:
    lDD->Delete();

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

// ===== Entry point ========================================================

void OnInterrupt(void* aContext, uint64_t aInterrupts)
{
    auto lInterruptCount = InterlockedIncrement(&sInterruptCount) - 1;
    assert(ITERATION > lInterruptCount);

    auto lSample = sSamples + lInterruptCount;

    lSample->OnInterrupt(aInterrupts);
}

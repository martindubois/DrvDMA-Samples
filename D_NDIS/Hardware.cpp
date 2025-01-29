
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Hardware.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "Hardware.h"

// Functions
// //////////////////////////////////////////////////////////////////////////

unsigned int Hardware_GetCurrentDuplexMode() { return 2; }

uint64_t Hardware_GetCurrentLinkSpeed() { return 10000000000; }

unsigned int Hardware_GetCurrentMediaState() { return 0; }

void Hardware_GetEthernetAddress(uint8_t* aOut)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        aOut[i] = i + 1;
    }
}

extern unsigned int Hardware_GetMaxRxPacketSize() { return 16384; }

extern uint64_t Hardware_GetMaxRxSpeed() { return 10000000000; }

extern uint64_t Hardware_GetMaxTxSpeed() { return 10000000000; }

extern unsigned int Hardware_GetMTU() { return 16384 - 14; }

extern void Hardware_Prepare() {}


// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_BAR/U_BAR.cpp

#define _KMS_WINDOWS_

// ===== Windows ============================================================
#include <Windows.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_U.h>

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define BAR_LOGICAL_INDEX (1)

#define OFFSET_byte (0x1000)
#define SIZE_byte   (0xd4)

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main()
{
    // Step 1 - Create an instance of the DrvDMA class
    auto lDD = DrvDMA::Create();

    // Step 2 - Connect the DrvDMA instance with the driver.
    // The argument is the index of the driver instance.
    auto lRet = lDD->Connect(0);
    if (DrvDMA_OK == lRet)
    {
        // Step 3 - Get address and size of the BAR
        auto lMem       = reinterpret_cast<volatile uint32_t*>(lDD->Memory_GetAddress(BAR_LOGICAL_INDEX));
        auto lSize_byte = lDD->Memory_GetSize(BAR_LOGICAL_INDEX);

        if ((nullptr == lMem) || (OFFSET_byte + SIZE_byte > lSize_byte))
        {
            printf("The configuration is not valid for the hardware\n");
            return 1;
        }

        // Step 4 - Read data from BAR and display them

        auto lCount  = SIZE_byte   / sizeof(uint32_t);
        auto lOffset = OFFSET_byte / sizeof(uint32_t);

        printf(" Address     Value\n");
        printf("---------- ----------\n");

        for (unsigned int i = 0; i < lCount; i++)
        {
            printf("0x%08x 0x%08x\n", static_cast<uint32_t>(OFFSET_byte + i * sizeof(uint32_t)), lMem[lOffset + i]);
        }
    }

    if (DrvDMA_OK != lRet)
    {
        printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
    }

    // Step 5 - Delete the DrvDMA instance
    lDD->Delete();

    return 0;
}

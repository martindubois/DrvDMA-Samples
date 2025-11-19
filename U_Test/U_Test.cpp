
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_XDMA_H2C/U_XDMA_H2C.cpp

#ifdef _KMS_LINUX_
    // ===== C ==============================================================
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>

    #define _stricmp strcmp
#endif

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== DrvDMA =============================================================
#include <DrvDMA_U.h>

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define DEVICE_INDEX (0)

// Data types
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    const char*  mName;
    unsigned int mOffset_byte;
}
RegInfo;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const char* MENU_DMA_ENGINE[] =
{
    "AMD XDMA",
    "Other",
    nullptr
};

#define MENU_AMD_XDMA (0)
#define MENU_OTHER    (1)

static const RegInfo AMD_XDMA_REGS[] =
{
    { "H2C 0        - ID", 0x00000000 },
    { "H2C 1        - ID", 0x00000100 },
    { "H2C 2        - ID", 0x00000200 },
    { "H2C 3        - ID", 0x00000300 },

    { "C2H 0        - ID", 0x00001000 },
    { "C2H 1        - ID", 0x00001100 },
    { "C2H 2        - ID", 0x00001200 },
    { "C2H 3        - ID", 0x00001300 },

    { "IRQ Block    - ID", 0x00002000 },

    { "Config       - ID", 0x00003000 },

    { "H2C SGDMA 0  - ID", 0x00004000 },
    { "H2C SGDMA 1  - ID", 0x00004100 },
    { "H2C SGDMA 2  - ID", 0x00004200 },
    { "H2C SGDMA 3  - ID", 0x00004300 },

    { "C2H SGDMA 0  - ID", 0x00005000 },
    { "C2H SGDMA 1  - ID", 0x00005100 },
    { "C2H SGDMA 2  - ID", 0x00005200 },
    { "C2H SGDMA 3  - ID", 0x00005300 },

    { "SGDMA Common - ID", 0x00006000 },
    { "MSI-X        - #0", 0x00008000 },

    { nullptr, 0 }
};

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static int AMD_XDMA(DrvDMA* aDD);

static void BeginStep(unsigned int* aStep, const char* aDesc);

static unsigned int Menu(const char** aMenu);

static void Pause();

static void Registers(volatile uint32_t* aPtr, unsigned int aSize_byte, const RegInfo* aInfos);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main()
{
    int lResult = __LINE__;

    unsigned int lStep = 0;

    BeginStep(&lStep, "Create an instance of the DrvDMA class");
    auto lDD = DrvDMA::Create();
    if (nullptr != lDD)
    {
        BeginStep(&lStep, "Retrieving library version");
        DrvDMA_Version lVersions[3];
        auto lCount = lDD->Versions(lVersions);
        for (unsigned int i = 0; i < lCount; i++)
        {
            printf("    Version %u\t", i);
            DrvDMA::Display(stdout, lVersions[i]);
        }
        if (1 != lCount)
        {
            printf("WARNING  DrvDMA::Versions failed - %u\n", lCount);
        }

        BeginStep(&lStep, "Verify the DrvDMA instance is not connected to the driver");
        auto lRetB = lDD->IsConnected();
        if (!lRetB)
        {
            BeginStep(&lStep, "Connect the DrvDMA instance with the driver");
            auto lRet = lDD->Connect(DEVICE_INDEX);
            if (DrvDMA_OK == lRet)
            {
                BeginStep(&lStep, "Verify the DrvDMA instance is connected to the driver");
                lRetB = lDD->IsConnected();
                if (lRetB)
                {
                    BeginStep(&lStep, "Retrieve driver versions");
                    lCount = lDD->Versions(lVersions);
                    for (unsigned int i = 0; i < lCount; i++)
                    {
                        printf("    Version %u\t", i);
                        DrvDMA::Display(stdout, lVersions[i]);
                    }
                    if (3 != lCount)
                    {
                        printf("WARNING  DrvDMA::Versions failed - %u\n", lCount);
                    }

                    BeginStep(&lStep, "Retrieve memory informations");
                    unsigned int lIndex = 0;
                    for (;;)
                    {
                        auto lSize_byte = lDD->Memory_GetSize(lIndex);
                        if (0 == lSize_byte)
                        {
                            printf("    %u memory regions\n", lIndex);
                            break;
                        }
                        if (0 != lSize_byte % 4096)
                        {
                            printf("WARNING  The memory region %i is smaller than a page\n", lIndex);
                        }
                        auto lAddress = reinterpret_cast<uint64_t>(lDD->Memory_GetAddress(lIndex));
                        if (0 == lAddress)
                        {
                            printf("WARNING  DrvDMA::Memory_GetAddress returned nullptr\n");
                        }
                        if (0 != lAddress % 4096)
                        {
                            printf("WARNING  The memory region %u address is not valid\n", lIndex);
                        }
                        printf("    Memory region %u - ", lIndex);

                        #ifdef _KMS_LINUX_
                            printf("0x%016lx", lAddress);
                        #endif

                        #ifdef _KMS_WINDOWS_
                            printf("0x%016llx", lAddress);
                        #endif

                        printf(", %u bytes\n", lSize_byte);

                        lIndex++;
                    }

                    BeginStep(&lStep, "Retrieve PCIe configuration space");
                    for (unsigned int i = 0; i < 9; i++)
                    {
                        uint32_t lValue;
                        lRet = lDD->PCIeConfig_Read(sizeof(uint32_t) * i, &lValue);
                        if (DrvDMA_OK != lRet)
                        {
                            printf("WARNING  DrvDMA::PCIeConfig_Read failed - %s\n", DrvDMA::GetResultName(lRet));
                            break;
                        }
                        printf("    0x%08x\n", lValue);
                    }

                    auto lChoice = Menu(MENU_DMA_ENGINE);
                    switch (lChoice)
                    {
                    case MENU_AMD_XDMA: lResult = AMD_XDMA(lDD); break;

                    default: lResult = 0;
                    }

                    lStep = 98;
                    BeginStep(&lStep, "DrvDMA::Disconnecting");
                    lRet = lDD->Disconnect();
                    if (DrvDMA_OK != lRet)
                    {
                        printf("ERROR  DrvDMA::Disconnect failed - %s\n", DrvDMA::GetResultName(lRet));
                        lResult = __LINE__;
                    }
                }
                else
                {
                    printf("FATAL ERROR  DrvDMA::IsConnected return false\n");
                    lResult = __LINE__;
                }
            }
            else
            {
                printf("FATAL ERROR  DrvDMA::Conect failed - %s\n", DrvDMA::GetResultName(lRet));
                lResult = __LINE__;
            }
        }
        else
        {
            printf("FATAL ERROR  DrvDMA::IsConnected returned true\n");
            lResult = __LINE__;
        }

        lStep = 99;
        BeginStep(&lStep, "Deleting the DrvDMA instance");
        lDD->Delete();
    }
    else
    {
        printf("FATAL ERROR  DrvDMA::Create failed\n");
        lResult = __LINE__;
    }

    printf("U_Test is returning %u\n", lResult);

    Pause();

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

int AMD_XDMA(DrvDMA* aDD)
{
    int lResult = 0;

    unsigned int lStep = 10;

    BeginStep(&lStep, "Identifying DMA control BAR");
    volatile uint32_t* lPtr;
    auto lSize_byte = aDD->Memory_GetSize(1);
    if (0 == lSize_byte)
    {
        lSize_byte = aDD->Memory_GetSize(0);
        lPtr = reinterpret_cast<volatile uint32_t*>(aDD->Memory_GetAddress(0));
    }
    else
    {
        lPtr = reinterpret_cast<volatile uint32_t*>(aDD->Memory_GetAddress(1));
    }
    if ((0 < lSize_byte) && (nullptr != lPtr))
    {
        Registers(lPtr, lSize_byte, AMD_XDMA_REGS);
    }
    else
    {
        printf("ERROR  No DMA control BAR\n");
        lResult = __LINE__;
    }

    return lResult;
}

void BeginStep(unsigned int* aStep, const char* aDesc)
{
    printf("\n%02u - %s\n", *aStep, aDesc);

    (*aStep)++;
}

unsigned int Menu(const char** aMenu)
{
    unsigned int lCount = 0;

    while (nullptr != aMenu[lCount])
    {
        printf("%u. %s\n", lCount, aMenu[lCount]);
        lCount++;
    }

    printf("A. Abort\n");

    unsigned int lResult = 0;

    for (;;)
    {
        printf("Your choice: ");

        char lLine[1024];

        auto lRet = fgets(lLine, sizeof(lLine), stdin);
        if (nullptr != lRet)
        {
            if (0 == _stricmp("A\n", lLine))
            {
                printf("Aborting\n");
                exit(__LINE__);
            }

            char* lEnd;

            lResult = strtoul(lLine, &lEnd, 10);
            if (('0' > *lEnd) && (lCount > lResult))
            {
                break;
            }
        }

        printf("Invalid choice, try again\n");
    }

    return lResult;
}

void Pause()
{
    printf("Press ENTER to continue\n");

    char lLine[1024];

    auto lRet = fgets(lLine, sizeof(lLine), stdin);
    (void)lRet;
}

void Registers(volatile uint32_t* aPtr, unsigned int aSize_byte, const RegInfo* aInfos)
{
    unsigned int lStep = 20;

    BeginStep(&lStep, "Displaying identification registers");

    unsigned int lIndex = 0;
    for (;;)
    {
        auto lInfo = aInfos + lIndex;
        if (nullptr == lInfo->mName)
        {
            break;
        }

        printf("    %s : ", lInfo->mName);

        if (aSize_byte >= (lInfo->mOffset_byte + sizeof(uint32_t)))
        {
            auto lRegIndex = lInfo->mOffset_byte / sizeof(uint32_t);

            auto lValue = aPtr[lRegIndex];

            printf("0x%08x\n", lValue);
        }
        else
        {
            printf("Invalid offset\n");
        }

        lIndex++;
    }
}

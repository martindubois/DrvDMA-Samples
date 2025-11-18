
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_XDMA_H2C/U_XDMA_H2C.cpp

#define _KMS_WINDOWS_

// ===== Windows ============================================================
#include <Windows.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_U.h>

// Data types
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    void* mAligned;
    void* mUnaligned;

    DrvDMA_Transfer_Status mStatus;
}
Buffer;

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define BAR_LOGICAL_INDEX (0)

// The theorical minimum length for the pipeline is 3: 1 buffer currently
// transferring from/to, 1 buffer in the driver for preparing the next
// transfer and 1 buffer in the application for processing.
// Increasing the number can help to deal with some small delay the
// operating system may cause.
// So, 4 buffers is the minimum recommanded when using buffer >= ~ 4 MiB.
// If the buffers are smaller, increassing the number can help.
#define BUFFER_QTY (4)

// CURRENT LIMITATION  DrvDMA currently limit transfer size to 32 MiB.
#define BUFFER_SIZE_byte (8 * 1024 * 1024)

#define ITERATION (100)

#define HARDWARE_ADDRESS (0x0000000000000000)

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Buffer_Alloc(Buffer* aBuffer);
static void Buffer_Free (Buffer* aBuffer);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main()
{
    printf("Step 1 - Create an instance of the DrvDMA class.\n");
    auto lDD = DrvDMA::Create();

    printf("Step 2 - Connect the DrvDMA instance with the driver.\n");
    // The argument is the index of the driver instance.
    auto lRet = lDD->Connect(0);
    if (DrvDMA_OK == lRet)
    {
        printf("Step 3 - Configure the DMA channel.\n");
        DrvDMA_Channel_Config lCfg;

        memset(&lCfg, 0, sizeof(lCfg));

        lCfg.mDescQty = 65536;
        lCfg.mEngine = DrvDMA_AMD_XDMA;
        lCfg.mFlags.mExclusive = true;
        lCfg.mFlags.mKernel = true;
        lCfg.mFlags.mToDevice = true;
        lCfg.mMem_Index = BAR_LOGICAL_INDEX;
        lCfg.mMem_Offset_byte = 0;

        unsigned int lCI;

        lRet = lDD->Channel_Config(lCfg, &lCI);
        if (DrvDMA_RESULT_OK(lRet))
        {
            printf("Step 4 - Allocating data buffers and fill them with data.\n");
            Buffer lBuffers[BUFFER_QTY];

            unsigned int i;

            for (i = 0; i < BUFFER_QTY; i++)
            {
                Buffer_Alloc(lBuffers + i);
            }

            printf("Step 5 - Prepare the electronic to receive data.\n");
            // Here, you must configure the user electronic present in the
            // FPGA. See the method Memory_GetAddress and Memory_GetSize

            uint64_t lStart_100ns;

            GetSystemTimePreciseAsFileTime(reinterpret_cast<LPFILETIME>(&lStart_100ns));

            // Step 6 - Fill the pipeline
            for (i = 0; i < BUFFER_QTY; i++)
            {
                auto lB = lBuffers + i;

                lRet = lDD->Start(lCI, false, lB->mAligned, 0, HARDWARE_ADDRESS, BUFFER_SIZE_byte, &lB->mStatus);
                if (DrvDMA_OK_PENDING != lRet)
                {
                    printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
                    exit(__LINE__);
                }
            }

            // Step 8 - Run the pipeline
            for (i = BUFFER_QTY; i < ITERATION; i++)
            {
                auto lB = lBuffers + (i % BUFFER_QTY);

                lRet = lDD->Wait(&lB->mStatus);
                if (DrvDMA_OK != lRet)
                {
                    printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
                    exit(__LINE__);
                }

                lRet = lDD->Start(lCI, false, lB->mAligned, 0, HARDWARE_ADDRESS, BUFFER_SIZE_byte, &lB->mStatus);
                if (DrvDMA_OK_PENDING != lRet)
                {
                    printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
                    exit(__LINE__);
                }
            }

            // Step 9 - Purge the pipeline
            for (i = 0; i < BUFFER_QTY; i++)
            {
                auto lB = lBuffers + i;

                lRet = lDD->Wait(&lB->mStatus);
                if (DrvDMA_OK)
                {
                    printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
                    exit(__LINE__);
                }
            }

            uint64_t lStop_100ns;

            GetSystemTimePreciseAsFileTime(reinterpret_cast<LPFILETIME>(&lStop_100ns));

            printf("Step 10 - Free buffers\n");
            for (i = 0; i < BUFFER_QTY; i++)
            {
                Buffer_Free(lBuffers + i);
            }

            double lTotal_byte = BUFFER_SIZE_byte * ITERATION;
            double lTotal_GB = lTotal_byte / 1000000000;
            double lDuration_100ns = static_cast<double>(lStop_100ns - lStart_100ns);
            double lDuration_s = lDuration_100ns / 10000000;
            double lSpeed_byte_s = lTotal_byte / lDuration_s;
            double lSpeed_GB_s = lSpeed_byte_s / 1000000000;

            printf("Total transfered : %f GB\n", lTotal_GB);
            printf("Duration : %f s\n", lDuration_s);
            printf("Speed: %f GB/s\n", lSpeed_GB_s);
        }
    }
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Buffer_Alloc(Buffer* aBuffer)
{
    auto lUnaligned = new uint8_t[BUFFER_SIZE_byte + 64];

    uint64_t lOffset_byte = 64 - (reinterpret_cast<uint64_t>(lUnaligned) % 64);

    aBuffer->mAligned = lUnaligned + lOffset_byte;
    aBuffer->mUnaligned = lUnaligned;

    memset(aBuffer->mUnaligned, 0, BUFFER_SIZE_byte);
}

static void Buffer_Free(Buffer* aBuffer)
{
    auto lUnaligned = reinterpret_cast<uint8_t*>(aBuffer->mUnaligned);

    delete[] lUnaligned;
}

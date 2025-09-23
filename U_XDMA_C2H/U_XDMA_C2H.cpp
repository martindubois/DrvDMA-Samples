
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      Sample_U_Simple/Sample_U_Simple.cpp

#define _KMS_WINDOWS_

// ===== Windows ============================================================
#include <Windows.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_U.h>

// Configurations
// //////////////////////////////////////////////////////////////////////////

// CURRENT LIMITATION  DrvDMA currently limit transfer size to 32 MiB.
#define BUFFER_SIZE_byte (16 * 1024 * 1024)

#define HARDWARE_ADDRESS (0x0000000000000000)

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
        // Step 3 - Configure the DMA channel
        // This sample configure the first H2C channel of a XDMA engine.
        // a. By setting mDescQty to 65536 we indicate to the driver to
        //    allocate space for 65536 descriptor. This is the maximum for a
        //    XDMA channel. We could indicate a smaller value to save kernel
        //    memory if the transfer are small.
        // b. By setting mFlags.mExclusize to true, we indicate to the driver
        //    to do not accept transfer request from other application.
        // c. By setting mFlags.mKernel to true, we indicate to the driver
        //    that it controls the DMA transfer.
        // d. By setting mFlags.mFromDevice to true, we indicate to the drive
        //    that we want to tranfer data from the device to the host memory
        //    (C2H).
        // e. By setting mMem_Index to 1, we indicate to the driver to use
        //    the second configured BAR to access XDMA registers.
        // f. By setting mMem_Offset_byte to 0x1000, we indicate to the
        //    driver to use this offset as base for the XDMA channel
        //    registers (first C2H channel)
        DrvDMA_Channel_Config lCfg;

        memset(&lCfg, 0, sizeof(lCfg));

        lCfg.mDescQty = 65536;
        lCfg.mEngine = DrvDMA_AMD_XDMA;
        lCfg.mFlags.mExclusive = true;
        lCfg.mFlags.mKernel = true;
        lCfg.mFlags.mFromDevice = true;
        lCfg.mMem_Index = 1;
        lCfg.mMem_Offset_byte = 0x1000;

        unsigned int lCI;

        lRet = lDD->Channel_Config(lCfg, &lCI);
        if (DrvDMA_RESULT_OK(lRet))
        {
            // The channel is now configured and the logical DMA channel
            // index is in lCI. Note that even if we use the first channel of
            // the XDMA engine, this index can be any integer value. We need
            // to consider it as an handle to the DMA channel.

            // Step 4 - Allocating data buffer
            // Buffer address must be aligned
            auto lBuffer = new uint8_t[BUFFER_SIZE_byte + 64];

            uint64_t lOffset_byte = 64 - (reinterpret_cast<uint64_t>(lBuffer) % 64);

            auto lAligned = lBuffer + lOffset_byte;

            // Step 5 - Writting data into the buffer
            memset(lAligned, 0, BUFFER_SIZE_byte);

            // Step 6 - Prepare the electronic to receive data
            // Here, you must configure the user electronic present in the
            // FPGA. See the method Memory_GetAddress and Memory_GetSize

            // Step 7 - Starting the DMA transfer
            // We could also use the Transfer method. This one start the
            // transfer and wait for its completion in a single method call.
            // There is some note about the arguments:
            // 1. lCI is the logical index of the channel to use
            // 2. true indicate the transfer is from the device to the host
            //    memory (C2H)
            // 3. 0 indicate to take data directly at the start of the
            //    buffer.
            // 4. HARDWARE_ADDRESS is the address on the internal AXI bus
            //    where the data must be written
            // 5. BUFFER_SIZE_byte is the size of the DMA tranfer.
            // 6. &lTS is the address of an instance of
            //    DrvDMA_Transfer_Status. The method put information about
            //    the DMA transfer in it.
            DrvDMA_Transfer_Status lTS;

            lRet = lDD->Start(lCI, true, lAligned, 0, HARDWARE_ADDRESS, BUFFER_SIZE_byte, &lTS);
            if (DrvDMA_OK_PENDING == lRet)
            {
                printf("Waiting...\n");

                // Step 8 - Wait for the completion of the DMA transfer
                lRet = lDD->Wait(&lTS);
            }

            // Step 9 - Release the data buffer
            delete[] lBuffer;
        }
    }

    if (DrvDMA_OK != lRet)
    {
        printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
    }

    // Step 10 - Delete the DrvDMA instance
    lDD->Delete();

    return 0;
}

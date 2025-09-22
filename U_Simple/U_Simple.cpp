
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2020-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      U_Simple/U_Simple.cpp

#ifdef _WIN32
    #define _KMS_WINDOWS_
#else
    #define _KMS_LINUX_
#endif

// ===== C ==================================================================
#include <string.h>

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== DrvDMA =============================================================
#include <DrvDMA_U.h>

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main()
{
    auto lDD = DrvDMA::Create();

    DrvDMA_Channel_Config lCfg;

    memset(&lCfg, 0, sizeof(lCfg));

    lCfg.mEngine            = DrvDMA_SIM;
    lCfg.mFlags.mFromDevice = true;
    lCfg.mFlags.mToDevice   = true;
    lCfg.mFlags.mUser       = true;

    unsigned int lCI;

    auto lRet = lDD->Channel_Config(lCfg, &lCI);
    if (DrvDMA_RESULT_OK(lRet))
    {
        uint8_t lBuffer[16384];
        DrvDMA_Transfer_Status lTS;

        lRet = lDD->Start(lCI, true, lBuffer, 0, 0, sizeof(lBuffer), &lTS);
        if (DrvDMA_OK_PENDING == lRet)
        {
            lRet = lDD->Wait(&lTS);
        }
    }

    if (DrvDMA_OK != lRet)
    {
        printf("Something got wrong (%s)\n", DrvDMA::GetResultName(lRet));
    }

    lDD->Delete();

    return 0;
}

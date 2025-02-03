
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Device_W.cpp

// References
// https://github.com/microsoft/NetAdapter-Cx-Driver-Samples/blob/release_2004/RtEthSample/power.cpp

#include "Component.h"

// ===== DrvDMA =============================================================

#ifdef _DEVICE_
    #include <DrvDMA_K_Windows.h>
#endif

#ifdef _SOFT_FUNC_
    #include <DrvDMA_K_SoftFunc_Windows.h>
#endif

// ===== Local ==============================================================
#include "Adapter.h"

#include "Device.h"

// Data types
// //////////////////////////////////////////////////////////////////////////

#define ADAPTER_QTY (1)

typedef struct
{
    // DrvDMA_Device or DrvDMA_SoftFunc member must be first

    #ifdef _DEVICE_
        DrvDMA_Device mDrvDMA_Device;
    #endif

    #ifdef _SOFT_FUNC_
        DrvDMA_SoftFunc mDrvDMA_SoftFunc;
    #endif

    Adapter* mAdapters[ADAPTER_QTY];
}
DeviceContext;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DeviceContext, GetDeviceContext);

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Hardware_Prepare(void* aSender, DrvDMA_CallbackType aType, void* aContext);

// Functions
// //////////////////////////////////////////////////////////////////////////

NTSTATUS Device_Create(WDFDEVICE_INIT* aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aInit);

    NetDeviceInitConfig(aInit);

    #ifdef _DEVICE_
        DrvDMA_Device_InitDeviceInit(aInit);
    #endif

    #ifdef _SOFT_FUNC_
        DrvDMA_SoftFunc_InitDeviceInit(aInit);
    #endif

    WDF_OBJECT_ATTRIBUTES lAttr;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&lAttr, DeviceContext);

    #ifdef _SOFT_FUNC_
        lAttr.EvtCleanupCallback = DrvDMA_SoftFunc_Cleanup;
    #endif

    WDFDEVICE lDevice;

    auto lResult = WdfDeviceCreate(&aInit, &lAttr, &lDevice);
    if (STATUS_SUCCESS == lResult)
    {
        ASSERT(nullptr != lDevice);

        auto lThis = GetDeviceContext(lDevice);
        ASSERT(nullptr != lThis);

        #ifdef _DEVICE_
            auto lRet = DrvDMA_Device_Create(&lThis->mDrvDMA_Device, lDevice);
        #endif

        #ifdef _SOFT_FUNC_
            auto lRet = DrvDMA_SoftFunc_Create(&lThis->mDrvDMA_SoftFunc, lDevice);
        #endif

        if (DrvDMA_OK == lRet)
        {
            #ifdef _DEVICE_
                DrvDMA_Device_Callback_Register(&lThis->mDrvDMA_Device, DrvDMA_CALLBACK_DEVICE_HARDWARE_PREPARE, nullptr, Hardware_Prepare, lThis);
            #endif

            for (unsigned int i = 0; i < ADAPTER_QTY; i++)
            {
                lResult = Adapter_Create(lDevice, lThis->mAdapters + i);
                if (STATUS_SUCCESS == lResult)
                {
                    #ifdef _SOFT_FUNC_
                        Adapter_Prepare(lThis->mAdapters[i]);
                    #endif
                }
                else
                {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - Adapter_Create( ,  ) failed - 0x%08x\n", lResult);
                    break;
                }
            }
        }
        else
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - DrvDMA_Device_Create( ,  ) failed - %u\n", lRet);
            lResult = STATUS_UNSUCCESSFUL;
        }
    }
    else
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - WdfDeviceCreate( , ,  ) failed - 0x%08x\n", lResult);
    }

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

#ifdef _DEVICE_

    void Hardware_Prepare(void* aSender, DrvDMA_CallbackType aType, void* aContext)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( , ,  )\n");

        ASSERT(nullptr != aContext);

        (void)aSender;
        (void)aType;

        auto lThis = reinterpret_cast<DeviceContext*>(aContext);

        for (unsigned int i = 0; i < ADAPTER_QTY; i++)
        {
            Adapter_Prepare(lThis->mAdapters[i]);
        }
    }

#endif


// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Device_W.cpp

// References
// https://github.com/microsoft/NetAdapter-Cx-Driver-Samples/blob/release_2004/RtEthSample/power.cpp

#include "Component.h"

// ===== DrvDMA =============================================================
#include <DrvDMA_K_Windows.h>

// ===== Local ==============================================================
#include "Adapter.h"

#include "Device.h"

// Data types
// //////////////////////////////////////////////////////////////////////////

#define ADAPTER_QTY (1)

typedef struct
{
    // DrvDMA_Device member must be first
    DrvDMA_Device mDrvDMA_Device;

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

    DrvDMA_Device_InitDeviceInit(aInit);

    WDF_OBJECT_ATTRIBUTES lAttr;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&lAttr, DeviceContext);

    WDFDEVICE lDevice;

    auto lResult = WdfDeviceCreate(&aInit, &lAttr, &lDevice);
    if (STATUS_SUCCESS == lResult)
    {
        ASSERT(nullptr != lDevice);

        auto lThis = GetDeviceContext(lDevice);
        ASSERT(nullptr != lThis);

        auto lRet = DrvDMA_Device_Create(&lThis->mDrvDMA_Device, lDevice);
        if (DrvDMA_OK == lRet)
        {
            DrvDMA_Device_Callback_Register(&lThis->mDrvDMA_Device, DrvDMA_CALLBACK_DEVICE_HARDWARE_PREPARE, &lThis->mDrvDMA_Device, Hardware_Prepare, lThis);

            for (unsigned int i = 0; i < ADAPTER_QTY; i++)
            {
                lResult = Adapter_Create(lDevice, lThis->mAdapters + i);
                if (STATUS_SUCCESS != lResult)
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

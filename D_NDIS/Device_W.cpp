
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
        DrvDMA_Func mDrvDMA_SoftFunc;
    #endif

    Adapter* mAdapters[ADAPTER_QTY];
}
DeviceContext;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DeviceContext, GetDeviceContext);

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

static EVT_WDF_DEVICE_D0_ENTRY         D0Entry;
static EVT_WDF_DEVICE_D0_EXIT          D0Exit;
static EVT_WDF_DEVICE_PREPARE_HARDWARE PrepareHardware;

// Functions
// //////////////////////////////////////////////////////////////////////////

NTSTATUS Device_Create(WDFDEVICE_INIT* aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aInit);

    NetDeviceInitConfig(aInit);

    WDF_PNPPOWER_EVENT_CALLBACKS lPnpPower;

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&lPnpPower);

    lPnpPower.EvtDeviceD0Entry         = D0Entry;
    lPnpPower.EvtDeviceD0Exit          = D0Exit;
    lPnpPower.EvtDevicePrepareHardware = PrepareHardware;

    #ifdef _DEVICE_

        DrvDMA_Device_InitDeviceInit(aInit);

        lPnpPower.EvtDeviceD0Exit          = DrvDMA_D0Exit;
        lPnpPower.EvtDeviceReleaseHardware = DrvDMA_ReleaseHardware;

    #endif

    #ifdef _SOFT_FUNC_

        // TODO

    #endif

    WdfDeviceInitSetPnpPowerEventCallbacks(aInit, &lPnpPower);

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

// ===== Entry points =======================================================

NTSTATUS D0Entry(WDFDEVICE aDevice, WDF_POWER_DEVICE_STATE aPreviousState)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    ASSERT(nullptr != aDevice);

    NTSTATUS lResult = STATUS_SUCCESS;

    #ifdef _DEVICE_
        lResult = DrvDMA_D0Entry(aDevice, aPreviousState);
    #endif

    #ifdef _SOFT_FUNC_
        (void)aPreviousState;
    #endif

    auto lThis = GetDeviceContext(aDevice);
    ASSERT(nullptr != lThis);

    for (unsigned int i = 0; i < ADAPTER_QTY; i++)
    {
        Adapter_UpdateCurrentState(lThis->mAdapters[i]);
    }

    return lResult;
}

NTSTATUS D0Exit(WDFDEVICE aDevice, WDF_POWER_DEVICE_STATE aTargetState)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    ASSERT(nullptr != aDevice);

    auto lThis = GetDeviceContext(aDevice);
    ASSERT(nullptr != lThis);

    for (unsigned int i = 0; i < ADAPTER_QTY; i++)
    {
        Adapter_UpdateUnknownState(lThis->mAdapters[i]);
    }

    NTSTATUS lResult = STATUS_SUCCESS;

    #ifdef _DEVICE_
        lResult = DrvDMA_D0Exit(aDevice, aTargetState);
    #endif

    #ifdef _SOFT_FUNC_
        (void)aTargetState;
    #endif

    return lResult;
}

NTSTATUS PrepareHardware(WDFDEVICE aDevice, WDFCMRESLIST aRaw, WDFCMRESLIST aTranslated)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( , ,  )\n");

    ASSERT(nullptr != aDevice);

    NTSTATUS lResult = STATUS_SUCCESS;

    #ifdef _DEVICE_
        lResult = DrvDMA_PrepareHardware(aDevice, aRaw, aTranslated);
    #endif

    #ifdef _SOFT_FUNC_
        (void)aRaw;
        (void)aTranslated;
    #endif

    auto lThis = GetDeviceContext(aDevice);
    ASSERT(nullptr != lThis);

    for (unsigned int i = 0; i < ADAPTER_QTY; i++)
    {
        Adapter_Prepare(lThis->mAdapters[i]);
    }

    return lResult;
}

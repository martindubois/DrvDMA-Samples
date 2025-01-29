
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Driver_W.cpp

// References
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/_netvista/
// https://learn.microsoft.com/en-us/windows-hardware/drivers/netcx/
// https://github.com/microsoft/NetAdapter-Cx-Driver-Samples/blob/release_2004/RtEthSample/driver.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "Device.h"
#include "Adapter.h"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

static EVT_WDF_DRIVER_DEVICE_ADD AddDevice;
static EVT_WDF_DRIVER_UNLOAD     Unload;

// Entry points
// //////////////////////////////////////////////////////////////////////////

extern "C"
{
    DRIVER_INITIALIZE DriverEntry;
}

#pragma alloc_text (INIT, DriverEntry)

NTSTATUS DriverEntry(PDRIVER_OBJECT aDriverObject, PUNICODE_STRING aRegistryPath)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    ASSERT(nullptr != aDriverObject);
    ASSERT(nullptr != aRegistryPath);

    WDF_DRIVER_CONFIG lConfig;

    WDF_DRIVER_CONFIG_INIT(&lConfig, AddDevice);

    lConfig.DriverPoolTag   = 'DvrD';
    lConfig.EvtDriverUnload = Unload;

    auto lResult = WdfDriverCreate(aDriverObject, aRegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &lConfig, nullptr);
    if (STATUS_SUCCESS != lResult)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - WdfDriverCreate( , , , ,  ) failed - 0x%08x\n", lResult);
    }

    return lResult;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

NTSTATUS AddDevice(WDFDRIVER aDriver, PWDFDEVICE_INIT aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    (void)aDriver;

    return Device_Create(aInit);
}

VOID Unload(WDFDRIVER aDriver)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aDriver;
}

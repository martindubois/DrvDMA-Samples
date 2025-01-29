
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Device.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

// aInit  The WDFDEVICE_INIT instance to use for creating the device
//
// Return
//  STATUS_SUCCESS
//  STATUS_UNSUCCESSFUL  See DrvDMA_Device_Create
//  ...                  See WdfDeviceCreate or Adapter_Create
extern NTSTATUS Device_Create(WDFDEVICE_INIT* aInit);

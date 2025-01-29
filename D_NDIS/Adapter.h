
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Adapter.h

#pragma once

// Data types
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    NETADAPTER mAdapter;
}
Adapter;

// Functions
// //////////////////////////////////////////////////////////////////////////

// aDevice   The parent of the adapter
// aAdapter  The function return the created instance there.
//
// Return
//  STATUS_SUCCESS
//  ...             See NetAdapterCreate
extern NTSTATUS Adapter_Create(WDFDEVICE aDevice, Adapter** aAdapter);

// aAdapter  The Adapter instance
extern void Adapter_Prepare(Adapter* aThis);

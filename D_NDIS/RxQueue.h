
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/RxQueue.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

// aCap  The NET_ADAPTER_RX_CAPABILITIES instance to initialize
extern void RxQueue_InitCapabilities(NET_ADAPTER_RX_CAPABILITIES* aCap);

// aInit  The NETRXQUEUE_INIT instante to use when creating the receive queue
//
// Return
//  STATUS_SUCCESS
//  ...             See NetRxQueueCreate
extern NTSTATUS RxQueue_Create(NETRXQUEUE_INIT* aInit);

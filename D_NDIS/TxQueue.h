
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/TxQueue.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

// aCap  The NET_ADAPTER_TX_CAPABILITIES instance to initialize
extern void TxQueue_InitCapabilities(NET_ADAPTER_TX_CAPABILITIES* aCap);

// aInit  The NETTXQUEUE_INIT instance to use when creating the transmit
//        queue
//
// Return
//  STATUS_SUCCESS
//  ...             See NetTxQueueCreate
extern NTSTATUS TxQueue_Create(NETTXQUEUE_INIT* aInit);

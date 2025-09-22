
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/TxQueue_W.cpp

// References
// https://github.com/microsoft/NetAdapter-Cx-Driver-Samples/blob/release_2004/RtEthSample/adapter.cpp

// NET_RING
//  BeginIndex -> First item the client driver own
//      DMA transfer pending
//  NextIndex  ->
//      DMA transfer to be programmed
//  EndIndex   -> Last item the client driver own

#include "Component.h"

// ===== Local ==============================================================
#include "TxQueue.h"

// Datatypes
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    const NET_RING_COLLECTION* mRings;
}
TxQueueContext;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(TxQueueContext, GetTxQueueContext);

// Static fonctions declarations
// //////////////////////////////////////////////////////////////////////////

static void IndicateTransmitedPackets(TxQueueContext* aThis);

// ===== Entry points =======================================================

static EVT_PACKET_QUEUE_ADVANCE                  Advance;
static EVT_PACKET_QUEUE_CANCEL                   Cancel;
static EVT_PACKET_QUEUE_SET_NOTIFICATION_ENABLED SetNotificationEnable;
static EVT_PACKET_QUEUE_START                    Start;
static EVT_PACKET_QUEUE_STOP                     Stop;

static EVT_WDF_OBJECT_CONTEXT_DESTROY Destroy;

// Functions
// //////////////////////////////////////////////////////////////////////////

void TxQueue_InitCapabilities(NET_ADAPTER_TX_CAPABILITIES* aCap)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aCap);

    NET_ADAPTER_TX_CAPABILITIES_INIT(aCap, 1);
}

NTSTATUS TxQueue_Create(NETTXQUEUE_INIT* aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aInit);

    NET_PACKET_QUEUE_CONFIG lConfig;

    NET_PACKET_QUEUE_CONFIG_INIT(&lConfig, Advance, SetNotificationEnable, Cancel);

    lConfig.EvtStart = Start;
    lConfig.EvtStop  = Stop;

    WDF_OBJECT_ATTRIBUTES lAttr;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&lAttr, TxQueueContext);

    lAttr.EvtDestroyCallback = Destroy;

    NETPACKETQUEUE lQueue;

    auto lResult = NetTxQueueCreate(aInit, &lAttr, &lConfig, &lQueue);
    if (STATUS_SUCCESS == lResult)
    {
        ASSERT(nullptr != lQueue);

        auto lThis = GetTxQueueContext(lQueue);
        ASSERT(nullptr != lThis);

        lThis->mRings = NetTxQueueGetRingCollection(lQueue);
        ASSERT(nullptr != lThis->mRings);
    }
    else
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - NetTxQueueCreate( , , ,  ) failed - 0x%08x\n", lResult);
    }

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void IndicateTransmitedPackets(TxQueueContext* aThis)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aThis->mRings);

    auto lPR = NetRingCollectionGetPacketRing(aThis->mRings);
    ASSERT(nullptr != lPR);

    auto lPI = lPR->BeginIndex;
    auto lPN = lPR->NextIndex;

    while (lPN != lPI)
    {
        // NOTE  Here, the driver verfy if the DMA transfer is completed.

        lPI = NetRingIncrementIndex(lPR, lPI);
    }
    lPR->BeginIndex = lPI;
}

// ===== Entry points =======================================================

void Advance(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aQueue);

    auto lThis = GetTxQueueContext(aQueue);

    auto lFR = NetRingCollectionGetFragmentRing(lThis->mRings);
    auto lPR = NetRingCollectionGetPacketRing  (lThis->mRings);

    // Program DMA for the packet to transmit

    auto lPE = lPR->EndIndex;
    auto lPI = lPR->NextIndex;

    while (lPE != lPI)
    {
        auto lP = NetRingGetPacketAtIndex(lPR, lPI);

        auto lFI = lP->FragmentIndex;

        for (unsigned int i = 0; i < lP->FragmentCount; i++)
        {
            // Here, the driver program the DMA transfer as needed

            lFI = NetRingIncrementIndex(lFR, lFI);
        }

        lFR->NextIndex = lFI;

        lPI = NetRingIncrementIndex(lPR, lPI);
    }
    lPR->NextIndex = lPI;

    IndicateTransmitedPackets(lThis);
}

void Cancel(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aQueue);

    auto lThis = GetTxQueueContext(aQueue);
    ASSERT(nullptr != lThis);

    // NOTE  If possible, the driver should cancel programmed DMA transfer.

    IndicateTransmitedPackets(lThis);
}

void SetNotificationEnable(NETPACKETQUEUE aQueue, BOOLEAN aEnabled)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    (void)aQueue;
    (void)aEnabled;

    // NOTE  Here, the driver enable end of transmit interrupt.
}

void Start(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aQueue;

    // NOTE  Here, the driver start the transmit queue.
}

void Stop(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aQueue;

    // NOTE  Here, the driver stop the transmit queue.
}

void Destroy(WDFOBJECT aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aQueue;
}

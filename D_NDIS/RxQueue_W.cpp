
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/RxQueue_W.cpp

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
#include "Hardware.h"

#include "RxQueue.h"

// Datatypes
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    const NET_RING_COLLECTION* mRings;
}
RxQueueContext;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(RxQueueContext, GetRxQueueContext);

// Static fonctions declarations
// //////////////////////////////////////////////////////////////////////////

static void IndicateReceivedPackets(RxQueueContext* aThis);

// ===== Entry points =======================================================

static EVT_PACKET_QUEUE_ADVANCE                  Advance;
static EVT_PACKET_QUEUE_CANCEL                   Cancel;
static EVT_PACKET_QUEUE_SET_NOTIFICATION_ENABLED SetNotificationEnable;
static EVT_PACKET_QUEUE_START                    Start;
static EVT_PACKET_QUEUE_STOP                     Stop;

static EVT_WDF_OBJECT_CONTEXT_DESTROY Destroy;

// Functions
// //////////////////////////////////////////////////////////////////////////

void RxQueue_InitCapabilities(NET_ADAPTER_RX_CAPABILITIES* aCap)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aCap);

    auto lMaxPacketSize_byte = Hardware_GetMaxRxPacketSize();
    ASSERT(0 < lMaxPacketSize_byte);

    NET_ADAPTER_RX_CAPABILITIES_INIT_SYSTEM_MANAGED(aCap, lMaxPacketSize_byte, 1);
}

NTSTATUS RxQueue_Create(NETRXQUEUE_INIT* aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aInit);

    NET_PACKET_QUEUE_CONFIG lConfig;

    NET_PACKET_QUEUE_CONFIG_INIT(&lConfig, Advance, SetNotificationEnable, Cancel);

    lConfig.EvtStart = Start;
    lConfig.EvtStop  = Stop;

    WDF_OBJECT_ATTRIBUTES lAttr;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&lAttr, RxQueueContext);

    lAttr.EvtDestroyCallback = Destroy;

    NETPACKETQUEUE lQueue;

    auto lResult = NetRxQueueCreate(aInit, &lAttr, &lConfig, &lQueue);
    if (STATUS_SUCCESS == lResult)
    {
        ASSERT(nullptr != lQueue);

        auto lThis = GetRxQueueContext(lQueue);
        ASSERT(nullptr != lThis);

        lThis->mRings = NetRxQueueGetRingCollection(lQueue);
        ASSERT(nullptr != lThis->mRings);
    }
    else
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - NetRxQueueCreate( , , ,  ) failed - 0x%08x\n", lResult);
    }

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void IndicateReceivedPackets(RxQueueContext* aThis)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aThis->mRings);

    auto lFR = NetRingCollectionGetFragmentRing(aThis->mRings);
    auto lPR = NetRingCollectionGetPacketRing  (aThis->mRings);
    ASSERT(nullptr != lFR);
    ASSERT(nullptr != lPR);

    auto lFI = lFR->BeginIndex;
    auto lFN = lFR->NextIndex;
    auto lPI = lPR->BeginIndex;

    while (lFN != lFI)
    {
        // NOTE  Here, the driver must verify if the hardware completed the
        //       programmed DMA transfer. If so, it must execute code similar
        //       to what is commented here.
        //
        // auto lF = NetRingGetFragmentAtIndex(lFR, lFI);
        // auto lP = NetRingGetPacketAtIndex  (lPR, lPI);
        //
        // lF->ValidLength = 0;
        // lF->Offset = 0;
        //
        // lP->FragmentCount = 1;
        // lP->FragmentIndex = 0;
        //
        // lFI = NetRingIncrementIndex(lFR, lFI);
        // lPI = NetRingIncrementIndex(lPR, lPI);
        break;
    }

    lFR->BeginIndex = lFI;
    lPR->BeginIndex = lPI;
}

// ===== Entry points =======================================================

void Advance(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aQueue);

    auto lThis = GetRxQueueContext(aQueue);
    ASSERT(nullptr != lThis);
    ASSERT(nullptr != lThis->mRings);

    IndicateReceivedPackets(lThis);

    // Program the DMA transfer for newly available fragments

    auto lFR = NetRingCollectionGetFragmentRing(lThis->mRings);
    ASSERT(nullptr != lFR);

    auto lFE = lFR->EndIndex;
    auto lFI = lFR->NextIndex;

    while (lFE != lFI)
    {
        // Here, the driver must program DMA descriptor as needed.

        lFI = NetRingIncrementIndex(lFR, lFI);
    }

    lFR->NextIndex = lFI;
}

void Cancel(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aQueue);

    auto lThis = GetRxQueueContext(aQueue);
    ASSERT(nullptr != lThis);
    ASSERT(nullptr != lThis->mRings);

    IndicateReceivedPackets(lThis);

    // Return packets packets indicating to ignore them

    auto lPR = NetRingCollectionGetPacketRing(lThis->mRings);
    ASSERT(nullptr != lPR);

    auto lPE = lPR->EndIndex;
    auto lPI = lPR->BeginIndex;

    while (lPE != lPI)
    {
        auto lP = NetRingGetPacketAtIndex(lPR, lPI);
        ASSERT(nullptr != lP);

        lP->Ignore = 1;

        lPI = NetRingIncrementIndex(lPR, lPI);
    }

    lPR->BeginIndex = lPI;

    // Return fragments

    auto lFR = NetRingCollectionGetFragmentRing(lThis->mRings);
    ASSERT(nullptr != lFR);

    auto lFE = lFR->EndIndex;
    auto lFI = lFR->BeginIndex;

    while (lFE != lFI)
    {
        lFI = NetRingIncrementIndex(lFR, lFI);
    }

    lFR->BeginIndex = lFI;
}

void SetNotificationEnable(NETPACKETQUEUE aQueue, BOOLEAN aEnabled)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    (void)aQueue;
    (void)aEnabled;
}

void Start(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aQueue;
}

void Stop(NETPACKETQUEUE aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aQueue;
}

void Destroy(WDFOBJECT aQueue)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    (void)aQueue;
}

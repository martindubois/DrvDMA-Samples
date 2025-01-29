
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Adapter_W.cpp

// References
// https://learn.microsoft.com/en-us/windows-hardware/drivers/netcx/device-and-adapter-initialization?source=recommendations
// https://github.com/microsoft/NetAdapter-Cx-Driver-Samples/blob/release_2004/RtEthSample/driver.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "Hardware.h"
#include "RxQueue.h"
#include "TxQueue.h"

#include "Adapter.h"

// Datatypes
// //////////////////////////////////////////////////////////////////////////

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(Adapter, GetAdapter);

// Static fonction declarations
// //////////////////////////////////////////////////////////////////////////

static void UpdateCurrentState(Adapter* aThis);

// ===== Entry points =======================================================

static EVT_NET_ADAPTER_CREATE_RXQUEUE CreateRxQueue;
static EVT_NET_ADAPTER_CREATE_TXQUEUE CreateTxQueue;

// Functions
// //////////////////////////////////////////////////////////////////////////

NTSTATUS Adapter_Create(WDFDEVICE aDevice, Adapter** aAdapter)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    ASSERT(nullptr != aDevice);
    ASSERT(nullptr != aAdapter);

    auto lInit = NetAdapterInitAllocate(aDevice);
    ASSERT(nullptr != lInit);

    NET_ADAPTER_DATAPATH_CALLBACKS lCallbacks;

    NET_ADAPTER_DATAPATH_CALLBACKS_INIT(&lCallbacks, CreateTxQueue, CreateRxQueue);

    NetAdapterInitSetDatapathCallbacks(lInit, &lCallbacks);

    WDF_OBJECT_ATTRIBUTES lAttr;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&lAttr, Adapter);

    NETADAPTER lAdapter;

    auto lResult = NetAdapterCreate(lInit, &lAttr, &lAdapter);

    NetAdapterInitFree(lInit);

    if (STATUS_SUCCESS == lResult)
    {
        ASSERT(nullptr != lAdapter);

        auto lThis = GetAdapter(lAdapter);
        ASSERT(nullptr != lThis);

        lThis->mAdapter = lAdapter;

        *aAdapter = lThis;
    }
    else
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ " - NetAdapterCreate failed - 0x%08x\n", lResult);
    }

    return lResult;
}

void Adapter_Prepare(Adapter* aThis)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aThis->mAdapter);

    auto lAdapter = aThis->mAdapter;

    NET_ADAPTER_RX_CAPABILITIES lRx;
    NET_ADAPTER_TX_CAPABILITIES lTx;

    RxQueue_InitCapabilities(&lRx);
    TxQueue_InitCapabilities(&lTx);

    NetAdapterSetDataPathCapabilities(lAdapter, &lTx, &lRx);

    auto lMaxRxSpeed_bit_s = Hardware_GetMaxRxSpeed();
    auto lMaxTxSpeed_bit_s = Hardware_GetMaxTxSpeed();

    ASSERT(0 < lMaxRxSpeed_bit_s);
    ASSERT(0 < lMaxTxSpeed_bit_s);

    NET_ADAPTER_LINK_LAYER_CAPABILITIES lLinkLayer;

    NET_ADAPTER_LINK_LAYER_CAPABILITIES_INIT(&lLinkLayer, lMaxTxSpeed_bit_s, lMaxRxSpeed_bit_s);

    NetAdapterSetLinkLayerCapabilities(lAdapter, &lLinkLayer);

    auto lMTU_byte = Hardware_GetMTU();
    ASSERT(0 < lMTU_byte);

    NetAdapterSetLinkLayerMtuSize(lAdapter, lMTU_byte);

    uint8_t lEthernetAddr[6];

    Hardware_GetEthernetAddress(lEthernetAddr);

    NET_ADAPTER_LINK_LAYER_ADDRESS lAddr;

    NET_ADAPTER_LINK_LAYER_ADDRESS_INIT(&lAddr, sizeof(lEthernetAddr), lEthernetAddr);

    NetAdapterSetPermanentLinkLayerAddress(lAdapter, &lAddr);

    NetAdapterSetCurrentLinkLayerAddress(lAdapter, &lAddr);

    Hardware_Prepare();

    UpdateCurrentState(aThis);

    auto lStatus = NetAdapterStart(lAdapter);
    ASSERT(STATUS_SUCCESS == lStatus);
    (void)lStatus;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void UpdateCurrentState(Adapter* aThis)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "(  )\n");

    ASSERT(nullptr != aThis->mAdapter);

    auto lCurrentLinkSpeed_bit_s = Hardware_GetCurrentLinkSpeed();
    auto lDuplexMode             = static_cast<NET_IF_MEDIA_DUPLEX_STATE>(Hardware_GetCurrentDuplexMode());
    auto lPause                  = NetAdapterPauseFunctionTypeUnsupported;
    auto lMediaState             = static_cast<NET_IF_MEDIA_CONNECT_STATE>(Hardware_GetCurrentMediaState());

    ASSERT(3 > lDuplexMode);
    ASSERT(2 > lMediaState);

    NET_ADAPTER_LINK_STATE lLinkState;

    NET_ADAPTER_LINK_STATE_INIT(&lLinkState, lCurrentLinkSpeed_bit_s, lMediaState, lDuplexMode, lPause,
        NetAdapterAutoNegotiationFlagXmitLinkSpeedAutoNegotiated
        | NetAdapterAutoNegotiationFlagRcvLinkSpeedautoNegotiated
        | NetAdapterAutoNegotiationFlagDuplexAutoNegotiated);

    NetAdapterSetLinkState(aThis->mAdapter, &lLinkState);
}

// ===== Entry points =======================================================

NTSTATUS CreateRxQueue(NETADAPTER aAdapter, NETRXQUEUE_INIT* aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    (void)aAdapter;

    return RxQueue_Create(aInit);
}

NTSTATUS CreateTxQueue(NETADAPTER aAdapter, NETTXQUEUE_INIT* aInit)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, __FUNCTION__ "( ,  )\n");

    (void)aAdapter;

    return TxQueue_Create(aInit);
}


// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Adapter.h

#pragma once

// ===== Linux kernel =======================================================
#include <linux/mii.h>
#include <linux/pci.h>

typedef struct
{
    struct net_device* mNetDev;
    struct pci_dev   * mPciDev;

    uint32_t mMsgLevel;
}
Adapter;

// Functions
// //////////////////////////////////////////////////////////////////////////

extern void Adapter_Create(Adapter* aThis, struct pci_dev* aPciDev);

extern void Adapter_Destroy(Adapter* aThis);

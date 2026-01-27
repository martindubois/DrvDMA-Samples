
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Adapter.h

#pragma once

typedef struct
{
    struct net_device * mNetDev;
}
Adapter;

// Functions
// //////////////////////////////////////////////////////////////////////////

extern void Adapter_Create(Adapter* aThis, struct pci_dev* aPciDev);

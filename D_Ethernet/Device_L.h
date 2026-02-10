
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Driver_L.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

extern int Device_Create(struct pci_dev* aDev, unsigned int aMajor, unsigned int aMinor, struct class* aClass);

extern void Device_Destroy(struct pci_dev* aDev);

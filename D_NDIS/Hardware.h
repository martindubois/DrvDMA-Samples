
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Hardware.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

// Return
//  0  Unknown
//  1  Half
//  2  Full
extern unsigned int Hardware_GetCurrentDuplexMode();

// Return  The current link speed in bit/s
extern uint64_t Hardware_GetCurrentLinkSpeed();

// Return
//  0  Connected
//  1  Disconnected
extern unsigned int Hardware_GetCurrentMediaState();

// aOut  The function return the ethernet address there
extern void Hardware_GetEthernetAddress(uint8_t* aOut);

// Return  The maximum packet size in byte
extern unsigned int Hardware_GetMaxRxPacketSize();

// Return  The maximum receive speed in bit/s
extern uint64_t Hardware_GetMaxRxSpeed();

// Return  The maximum transmit speed in bit/s
extern uint64_t Hardware_GetMaxTxSpeed();

// Return  The MDU in byte
extern unsigned int Hardware_GetMTU();

extern void Hardware_Prepare();

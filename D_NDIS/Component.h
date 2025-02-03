
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Component.h

#pragma once

#define _KMS_WINDOWS_

// Define _DEVICE_ or _SOFT_FUNC_
// _DEVICE_    The device is enumerated by the PCI sub system
// _SOFT_FUNC_ The DrvDMA driver enumerate the configured soft function

// #define _DEVICE_
#define _SOFT_FUNC_

// ===== WDM ================================================================

#define INITGUID

#include <ntddk.h>

// ===== WDF ================================================================
#include <wdf.h>
#include <wdftypes.h>
#include <ntintsafe.h>

#include <netadaptercx.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_StdTypes.h>

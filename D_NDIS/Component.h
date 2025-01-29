
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMA
// File      D_NDIS/Component.h

#pragma once

#define _KMS_WINDOWS_

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

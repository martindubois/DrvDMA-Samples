
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_SoftFunc_L/Device_L.c

#include "Component.h"

// ===== DrvDMA =============================================================
#include <DrvDMA_K_SoftFunc_Linux.h>

// ===== Local ==============================================================
#include "Device_L.h"

// Data types
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    DrvDMA_Func mDrvDMA_Func;

    struct auxiliary_device* mAuxiliaryDevice;
}
DeviceContext;

// Functions
// //////////////////////////////////////////////////////////////////////////

int Device_Create(struct auxiliary_device* aAuxiliary)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    DeviceContext* lThis = kmalloc(sizeof(DeviceContext), GFP_KERNEL);

    memset(lThis, 0, sizeof(*lThis));

    lThis->mAuxiliaryDevice = aAuxiliary;

    DrvDMA_Result lRet = DrvDMA_SoftFunc_Create(&lThis->mDrvDMA_Func, aAuxiliary);
    if (DrvDMA_OK == lRet)
    {
        DrvDMA_Version lV;

        DrvDMA_Func_GetVersion(&lThis->mDrvDMA_Func, &lV);

        printk(KERN_INFO PREFIX "%s %u.%u.%u.%u %s\n", lV.mComponent, lV.mMajor, lV.mMinor, lV.mBuild, lV.mCompat, lV.mComment);

        DrvDMA_Device_Config lC;

        DrvDMA_Func_Device_Config_Get(&lThis->mDrvDMA_Func, &lC);

        printk(KERN_INFO PREFIX "Soft bus %s, Device type %u\n", lC.mFlags.mSoftBus_Enable ? "enabled" : "disabled", lC.mDeviceType);

        DrvDMA_Device_Info lI;

        DrvDMA_Func_Device_Info_Get(&lThis->mDrvDMA_Func, &lI);

        printk(KERN_INFO PREFIX "%s, %s\n", lI.mHardwareId, lI.mLocation);
    }

    return (DrvDMA_OK == lRet) ? 0 : - __LINE__;
}

void Device_Destroy(struct auxiliary_device* aAuxiliary)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    DrvDMA_Func* lFunc = DrvDMA_SoftFunc_FromAuxiliary(aAuxiliary);

    DeviceContext* lThis = (DeviceContext*)lFunc;

    DrvDMA_SoftFunc_Destroy(&lThis->mDrvDMA_Func);
}

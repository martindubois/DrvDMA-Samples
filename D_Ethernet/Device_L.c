
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Driver_L.c

#include "Component.h"

// ===== Linux ==============================================================
#include <linux/etherdevice.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_K_Linux.h>

// ===== Local ==============================================================
#include "Adapter.h"

#include "Device_L.h"

// Data types
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    DrvDMA_Device mDrvDMA_Device;

    Adapter          * mAdapter;
    struct net_device* mNetDevice;
}
DeviceContext;

// Static variables
// //////////////////////////////////////////////////////////////////////////

static unsigned int sDeviceCount = 0;

static struct file_operations sOperations =
{
    .owner          = THIS_MODULE,
    .open           = (void *)DrvDMA_Open,
    .mmap           = (void *)DrvDMA_MMap,
    .release        = (void *)DrvDMA_Release,
    .unlocked_ioctl = (void *)DrvDMA_IoCtl,
};

// Functions
// //////////////////////////////////////////////////////////////////////////

int Device_Create(struct pci_dev* aDev, unsigned int aMajor, unsigned int aMinor, struct class* aClass)
{
    static const DrvDMA_Version VERSION = { 1, 0, 0, 0, "D_Ethernet.ko", "sample" };

    printk(KERN_DEBUG PREFIX "%s( , %u, %u )\n", __FUNCTION__, aMajor, aMinor);

    DeviceContext* lThis = kmalloc(sizeof(DeviceContext), GFP_KERNEL);

    unsigned int lMinor = aMinor + sDeviceCount;
    dev_t        lDevId = MKDEV(aMajor, lMinor);

    int lResult = DrvDMA_Device_Create(&lThis->mDrvDMA_Device, aDev, lDevId, lMinor, sDeviceCount, aClass, &sOperations);
    if (0 == lResult)
    {
        DrvDMA_Device_SetDriverVersion(&lThis->mDrvDMA_Device, &VERSION);

        DrvDMA_Result lRet = DrvDMA_Device_PrepareHardware(&lThis->mDrvDMA_Device);
        if (DrvDMA_OK == lRet)
        {
            sDeviceCount++;

            lThis->mNetDevice = alloc_etherdev(sizeof(Adapter));
            if (NULL != lThis->mNetDevice)
            {
                pci_set_drvdata(aDev, lThis);

                lThis->mAdapter = netdev_priv(lThis->mNetDevice);

                lThis->mAdapter->mNetDev = lThis->mNetDevice;

                Adapter_Create(lThis->mAdapter, aDev);
            }
            else
            {
                printk(KERN_ERR PREFIX "%s - ENOMEM\n", __FUNCTION__);
                DrvDMA_Device_ReleaseHardware(&lThis->mDrvDMA_Device);
                return - ENOMEM;
            }
        }
        else
        {
            printk(KERN_ERR PREFIX "%s - DrvDMA_Device_PrepareHardware(  ) failed - %u\n", __FUNCTION__, lRet);
            lResult = - __LINE__;
        }

        if (0 != lResult)
        {
            DrvDMA_Device_Delete(&lThis->mDrvDMA_Device);
        }
    }
    else
    {
        printk(KERN_ERR PREFIX "%s - DrvDMA_Device_Create failed\n", __FUNCTION__);
    }

    if (0 != lResult)
    {
        kfree(lThis);
    }

    return lResult;
}

void Device_Destroy(struct pci_dev* aDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    DeviceContext* lThis = pci_get_drvdata(aDev);

    Adapter_Destroy(lThis->mAdapter);

    free_netdev(lThis->mNetDevice);

    DrvDMA_Device_ReleaseHardware(&lThis->mDrvDMA_Device);

    DrvDMA_Device_Delete(&lThis->mDrvDMA_Device);

    kfree(lThis);
}

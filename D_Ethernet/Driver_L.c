
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Driver_L.c

#include "Component.h"

// ===== Linux kernel =======================================================
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/pci.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_K_Linux.h>

// ===== Local ==============================================================
#include "Adapter.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

#define DEVICE_COUNT_MAX (16)

static struct pci_device_id ID_TABLE[] =
{
    { PCI_DEVICE(0x10ee, 0x9034), }, // PT

    { 0, }
};

MODULE_DEVICE_TABLE(pci, ID_TABLE);

#define MODULE_NAME "D_Ethernet"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

static char * DevNode(const struct device * aDev, umode_t * aMode);

static void __exit Exit(void);
static int  __init Init(void);

static int  Probe (struct pci_dev * aPciDev, const struct pci_device_id * aId);
static void Remove(struct pci_dev * aPciDev);

// Static variables
// //////////////////////////////////////////////////////////////////////////

static struct file_operations sOperations =
{
    .owner          = THIS_MODULE,
    .open           = (void *)DrvDMA_Open,
    .mmap           = (void *)DrvDMA_MMap,
    .release        = (void *)DrvDMA_Release,
    .unlocked_ioctl = (void *)DrvDMA_IoCtl,
};

static struct pci_driver sPciDriver =
{
    .name     = MODULE_NAME,
    .id_table = ID_TABLE,
    .probe    = Probe,
    .remove   = Remove,
};

static dev_t sChrDev;

static struct class * sClass = NULL;

static unsigned int sDeviceCount = 0;

// Static functions
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

char * DevNode(const struct device * aDev, umode_t * aMode)
{
    if (NULL != aMode)
    {
        (*aMode) = 0666;
    }

    return NULL;
}

void Exit()
{
    printk(KERN_DEBUG PREFIX "%s()\n", __FUNCTION__);

    pci_unregister_driver(&sPciDriver);
}

module_exit(Exit);

int Init()
{
    int lResult = 0;

    printk(KERN_DEBUG PREFIX "%s()\n", __FUNCTION__);

    sClass = class_create("DrvDMA");
    if (NULL == sClass)
    {
        printk(KERN_ERR PREFIX "%s - class_create(  ) failed", __FUNCTION__);
        lResult = - __LINE__;
    }
    else
    {
        sClass->devnode = DevNode;

        int lRet = alloc_chrdev_region(&sChrDev, 0, DEVICE_COUNT_MAX, MODULE_NAME);
        if (0 != lRet)
        {
            printk(KERN_ERR PREFIX "%s - alloc_chrdev_region( , , ,  ) failed - %d\n", __FUNCTION__, lRet);
            lResult = __LINE__;
        }
        else
        {
            lRet = pci_register_driver(&sPciDriver);
            if (0 != lRet)
            {
                printk(KERN_ERR PREFIX "%s - pci_register_driver(  ) failed - %d\n", __FUNCTION__, lRet);
                unregister_chrdev_region(sChrDev, DEVICE_COUNT_MAX);
                lResult = __LINE__;
            }
        }

        if (0 != lResult)
        {
            class_destroy(sClass);
        }
    }

    return lResult;
}

module_init(Init);

int Probe(struct pci_dev * aPciDev, const struct pci_device_id * aId)
{
    static const DrvDMA_Version VERSION = { 1, 0, 0, 0, "D_Ethernet.ko", "sample" };

    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    unsigned int lMinor = MINOR(sChrDev) + sDeviceCount;
    dev_t        lDevId = MKDEV(MAJOR(sChrDev), lMinor);

    DrvDMA_Device* lDevice = DrvDMA_Device_Create(aPciDev, lDevId, lMinor, sDeviceCount, sClass, &sOperations);
    if (NULL == lDevice)
    {
        printk(KERN_ERR PREFIX "%s - DrvDMA_Device_Create failed\n", __FUNCTION__);
        return - __LINE__;
    }

    DrvDMA_Device_SetDriverVersion(lDevice, &VERSION);

    DrvDMA_Result lRet = DrvDMA_Device_PrepareHardware(lDevice);
    if (DrvDMA_OK != lRet)
    {
        printk(KERN_ERR PREFIX "%s - DrvDMA_Device_PrepareHardware(  ) failed - %u\n", __FUNCTION__, lRet);
        DrvDMA_Device_Delete(lDevice);
        return - __LINE__;
    }

    sDeviceCount++;

    struct net_device * lNetDev = alloc_etherdev(sizeof(Adapter));
    if (NULL == lNetDev)
    {
        printk(KERN_ERR PREFIX "%s - ENOMEM\n", __FUNCTION__);
        DrvDMA_Device_ReleaseHardware(lDevice);
        DrvDMA_Device_Delete(lDevice);
        return -ENOMEM;
    }

    Adapter* lAdapter = netdev_priv(lNetDev);

    lAdapter->mNetDev = lNetDev;

    Adapter_Create(lAdapter, aPciDev);
    
    return 0;
}

void Remove(struct pci_dev * aPciDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    DrvDMA_Device* lDevice = DrvDMA_Device_Find(aPciDev);

    DrvDMA_Device_ReleaseHardware(lDevice);

    DrvDMA_Device_Delete(lDevice);
}

// License
/////////////////////////////////////////////////////////////////////////////

MODULE_LICENSE("GPL");

MODULE_AUTHOR("KMS - Martin Dubois, P. Eng. <mdubois@kms-quebec.com>");
MODULE_DESCRIPTION("D_Ethernet");

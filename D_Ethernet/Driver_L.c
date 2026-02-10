
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025-2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Driver_L.c

#include "Component.h"

// ===== Linux kernel =======================================================
#include <linux/module.h>
#include <linux/pci.h>

// ===== DrvDMA =============================================================
#include <DrvDMA_K_Linux.h>

// ===== Local ==============================================================
#include "Device_L.h"

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

static int  Probe (struct pci_dev * aDev, const struct pci_device_id * aId);
static void Remove(struct pci_dev * aDev);

// Static variables
// //////////////////////////////////////////////////////////////////////////

static struct pci_driver sPciDriver =
{
    .name     = MODULE_NAME,
    .id_table = ID_TABLE,
    .probe    = Probe,
    .remove   = Remove,
};

static dev_t sChrDev;

static struct class * sClass = NULL;

// Static functions
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

char* DevNode(const struct device * aDev, umode_t * aMode)
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

    unregister_chrdev_region(sChrDev, DEVICE_COUNT_MAX);

    class_destroy(sClass);
}

module_exit(Exit);

int Init()
{
    int lResult = 0;

    printk(KERN_DEBUG PREFIX "%s()\n", __FUNCTION__);

    sClass = class_create("D_Ethernet");
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

int Probe(struct pci_dev * aDev, const struct pci_device_id *)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    return Device_Create(aDev, MAJOR(sChrDev), MINOR(sChrDev), sClass);
}

void Remove(struct pci_dev * aDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    Device_Destroy(aDev);
}

// License
/////////////////////////////////////////////////////////////////////////////

MODULE_LICENSE("GPL");

MODULE_AUTHOR("KMS - Martin Dubois, P. Eng. <mdubois@kms-quebec.com>");
MODULE_DESCRIPTION("D_Ethernet");

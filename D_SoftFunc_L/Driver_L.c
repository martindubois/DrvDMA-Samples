
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_SoftFunc_L/Driver_L.c

#include "Component.h"

// ===== Linux kernel =======================================================
#include <linux/device.h>
#include <linux/module.h>

// ===== Local ==============================================================
#include "Device_L.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

#define DEVICE_COUNT_MAX (16)

static const struct auxiliary_device_id ID_TABLE[] =
{
    { .name = "DrvDMA.toto" },

    {}
};

MODULE_DEVICE_TABLE(auxiliary, ID_TABLE);

#define MODULE_NAME "D_SoftFunc_L"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

static void __exit Exit(void);
static int  __init Init(void);

static int  Probe (struct auxiliary_device* aDev, const struct auxiliary_device_id* aId);
static void Remove(struct auxiliary_device* aDev);

// Static variables
// //////////////////////////////////////////////////////////////////////////

static struct auxiliary_driver sAuxiliaryDriver =
{
    .name     = MODULE_NAME,
    .id_table = ID_TABLE,
    .probe    = Probe,
    .remove   = Remove,
};

// Static functions
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================

void Exit()
{
    printk(KERN_DEBUG PREFIX "%s()\n", __FUNCTION__);

    auxiliary_driver_unregister(&sAuxiliaryDriver);
}

module_exit(Exit);

int Init()
{
    printk(KERN_DEBUG PREFIX "%s()\n", __FUNCTION__);

    int lResult = auxiliary_driver_register(&sAuxiliaryDriver);
    if (0 != lResult)
    {
        printk(KERN_ERR PREFIX "%s - auxiliary_driver_register(  ) failed - %d\n", __FUNCTION__, lResult);
    }

    return lResult;
}

module_init(Init);

int Probe(struct auxiliary_device* aDev, const struct auxiliary_device_id* aId)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    return Device_Create(aDev);
}

void Remove(struct auxiliary_device* aDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    Device_Destroy(aDev);
}

// License
/////////////////////////////////////////////////////////////////////////////

MODULE_LICENSE("GPL");

MODULE_AUTHOR("KMS - Martin Dubois, P. Eng. <mdubois@kms-quebec.com>");
MODULE_DESCRIPTION("D_SoftFunc_L");

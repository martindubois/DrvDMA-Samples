
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2025 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   DrvDMS-Sample
// File      D_Ethernet/Adapter_L.c

#include "Component.h"

// ===== Linux kernel =======================================================
#include <linux/etherdevice.h>
#include <linux/ethtool.h>

// ===== Local ==============================================================
#include "Adapter.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

#define WATCHDOG_tick (2 * HZ)

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points - EthTool =============================================
static void     GetDrvInfo      (struct net_device* anetDev, struct ethtool_drvinfo* aOut);
static int      GetEeprom       (struct net_device* aNetDev, struct ethtool_eeprom* aEeprom, uint8_t* aOut);
static int      GetEepromLen    (struct net_device* anetDev);
static void     GetEthToolStats (struct net_device* aNetDev, struct ethtool_stats* aStats, uint64_t* aOut);
static uint32_t GetLink         (struct net_device* aNetDev);
static int      GetLinkKSettings(struct net_device* aNetDev, struct ethtool_link_ksettings* aOut);
static uint32_t GetMsgLevel     (struct net_device* aNetDev);
static void     GetRegs         (struct net_device* aNetDev, struct ethtool_regs* aRegs, void* aPtr);
static int      GetRegsLen      (struct net_device* aNetDev);
static void     GetRingParam    (struct net_device* aNetDev, struct ethtool_ringparam* aParam, struct kernel_ethtool_ringparam* aKernelParam, struct netlink_ext_ack* aExtParam);
static int      GetSSetCount    (struct net_device* aNetDev, int aSSet);
static void     GetStrings      (struct net_device* aNetDev, uint32_t aIndex, uint8_t* aOut);
static void     GetWoL          (struct net_device* aNetDev, struct ethtool_wolinfo* aOut);
static int      NWayReset       (struct net_device* aNetDev);
static void     SelfTest        (struct net_device* aNetDev, struct ethtool_test* aTest, uint64_t* aOut);
static int      SetEeprom       (struct net_device* aNetDev, struct ethtool_eeprom* aEeprom, uint8_t* aIn);
static int      SetLinkKSettings(struct net_device* aNetDev, const struct ethtool_link_ksettings* aIn);
static void     SetMsgLevel     (struct net_device* aNetDev, uint32_t aValue);
static int      SetPhysId       (struct net_device* aNetDev, enum ethtool_phys_id_state aState);
static int      SetRingParam    (struct net_device* aNetDev, struct ethtool_ringparam* aParam, struct kernel_ethtool_ringparam* aKernelParam, struct netlink_ext_ack* aExtParam);
static int      SetWoL          (struct net_device* aNetDev, struct ethtool_wolinfo* aIn);

// ===== Entry points - Net Device ==========================================
static int  EthIoCtl     (struct net_device* aNetDev, struct ifreq* aRequest, int aCmd);
static int  Open         (struct net_device* aNetDev);
static int  SetFeatures  (struct net_device* aNetDev, netdev_features_t aFeatures);
static int  SetMacAddress(struct net_device* aNetDev, void* aAddr);
static void SetRxMode    (struct net_device* aNetDev);
static int  Stop         (struct net_device* aNetDev);
static void TxTimeout    (struct net_device* aNetDev, unsigned int aTxQueue);

static netdev_tx_t StartXmit(struct sk_buff* aBuffer, struct net_device* aNetDev);

// Static variables
// //////////////////////////////////////////////////////////////////////////

static const struct ethtool_ops sOperations_EthTool =
{
    .get_drvinfo        = GetDrvInfo,
    .get_eeprom         = GetEeprom,
    .get_eeprom_len     = GetEepromLen,
    .get_ethtool_stats  = GetEthToolStats,
    .get_link           = GetLink,
    .get_link_ksettings = GetLinkKSettings,
    .get_msglevel       = GetMsgLevel,
    .get_regs_len       = GetRegsLen,
    .get_regs           = GetRegs,
    .get_ringparam      = GetRingParam,
    .get_sset_count     = GetSSetCount,
    .get_strings        = GetStrings,
    .get_ts_info        = ethtool_op_get_ts_info,
    .get_wol            = GetWoL,
    .nway_reset         = NWayReset,
    .self_test          = SelfTest,
    .set_eeprom         = SetEeprom,
    .set_link_ksettings = SetLinkKSettings,
    .set_msglevel       = SetMsgLevel,
    .set_phys_id        = SetPhysId,
    .set_ringparam      = SetRingParam,
    .set_wol            = SetWoL,
};

static const struct net_device_ops sOperations_NetDev =
{
    .ndo_eth_ioctl       = EthIoCtl,
    .ndo_open            = Open,
    .ndo_set_features    = SetFeatures,
    .ndo_set_mac_address = SetMacAddress,
    .ndo_set_rx_mode     = SetRxMode,
    .ndo_start_xmit      = StartXmit,
    .ndo_stop            = Stop,
    .ndo_tx_timeout      = TxTimeout,
    .ndo_validate_addr   = eth_validate_addr,
};

// Functions
// //////////////////////////////////////////////////////////////////////////

void Adapter_Create(Adapter* aThis, struct pci_dev* aPciDev)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    struct net_device* lNetDev = aThis->mNetDev;

    lNetDev->ethtool_ops    = &sOperations_EthTool;
    lNetDev->hw_features   |= NETIF_F_RXALL | NETIF_F_RXFCS;
    lNetDev->netdev_ops     = &sOperations_NetDev;
    lNetDev->priv_flags    |= IFF_SUPP_NOFCS;
    lNetDev->watchdog_timeo = WATCHDOG_tick;

    strscpy(lNetDev->name, "eth%d");

    int lRet = register_netdev(lNetDev);
    if (0 != lRet)
    {
        printk(KERN_ERR PREFIX "%s - register_netdev(  ) failed - %d\n", __FUNCTION__, lRet);
        return;
    }
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points - EthTool =============================================

void GetDrvInfo(struct net_device* anetDev, struct ethtool_drvinfo* aOut)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
}

int GetEeprom(struct net_device* aNetDev, struct ethtool_eeprom* aEeprom, uint8_t* aOut)
{
    printk(KERN_DEBUG PREFIX "%s( , ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int GetEepromLen(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

void GetEthToolStats(struct net_device* aNetDev, struct ethtool_stats* aStats, uint64_t* aOut)
{    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    // TODO

    printk(KERN_DEBUG PREFIX "%s( , ,  )\n", __FUNCTION__);
    
    // TODO
}

uint32_t GetLink(struct net_device* aNetDev)
{printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    // TODO
    return 0;
}

int GetLinkKSettings(struct net_device* aNetDev, struct ethtool_link_ksettings* aOut)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

uint32_t GetMsgLevel(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    // TODO
    return 0;
}

void GetRegs(struct net_device* aNetDev, struct ethtool_regs* aRegs, void* aPtr)
{
    printk(KERN_DEBUG PREFIX "%s( , ,  )\n", __FUNCTION__);
    
    // TODO
}

int GetRegsLen(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

void GetRingParam(struct net_device* aNetDev, struct ethtool_ringparam* aParam, struct kernel_ethtool_ringparam* aKernelParam, struct netlink_ext_ack* aExtParam)
{
    printk(KERN_DEBUG PREFIX "%s( , , ,  )\n", __FUNCTION__);
    
    // TODO
}

int GetSSetCount(struct net_device* aNetDev, int aSSet)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

void GetStrings(struct net_device* aNetDev, uint32_t aIndex, uint8_t* aOut)
{
    printk(KERN_DEBUG PREFIX "%s( , ,  )\n", __FUNCTION__);
    
    // TODO
}

void GetWoL(struct net_device* aNetDev, struct ethtool_wolinfo* aOut)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
}

int NWayReset(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

void SelfTest(struct net_device* aNetDev, struct ethtool_test* aTest, uint64_t* aOut)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
}

int SetEeprom(struct net_device* aNetDev, struct ethtool_eeprom* aEeprom, uint8_t* aIn)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int SetLinkKSettings(struct net_device* aNetDev, const struct ethtool_link_ksettings* aIn)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

void SetMsgLevel(struct net_device* aNetDev, uint32_t aValue)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
}

int SetPhysId(struct net_device* aNetDev, enum ethtool_phys_id_state aState)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int SetRingParam(struct net_device* aNetDev, struct ethtool_ringparam* aParam, struct kernel_ethtool_ringparam* aKernelParam, struct netlink_ext_ack* aExtParam)
{
    printk(KERN_DEBUG PREFIX "%s( , , ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int SetWoL(struct net_device* aNetDev, struct ethtool_wolinfo* aIn)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

// ===== Entry points - Net Device ==========================================

int EthIoCtl(struct net_device* aNetDev, struct ifreq* aRequest, int aCmd)
{
    printk(KERN_DEBUG PREFIX "%s( , ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int Open(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int SetFeatures(struct net_device* aNetDev, netdev_features_t aFeatures)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // TODO
    return - __LINE__;
}

int SetMacAddress(struct net_device* aNetDev, void* aAddr)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    // TODO
    return - __LINE__;
}

void SetRxMode(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    // TODO
}

int Stop(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    // TODO
    return - __LINE__;
}

void TxTimeout(struct net_device* aNetDev, unsigned int aTxQueue)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    // TODO
}

netdev_tx_t StartXmit(struct sk_buff* aBuffer, struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    // TODO
    return NETDEV_TX_OK;
}

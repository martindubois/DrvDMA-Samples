
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
static uint32_t GetMsgLevel (struct net_device* aNetDev);
static int      GetSSetCount(struct net_device* aNetDev, int aSSet);
static int      NWayReset   (struct net_device* aNetDev);
static int      ReturnZero  (struct net_device* anetDev);
static void     SetMsgLevel (struct net_device* aNetDev, uint32_t aValue);

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
    .get_eeprom_len = ReturnZero,
    .get_link       = ethtool_op_get_link,
    .get_msglevel   = GetMsgLevel,
    .get_regs_len   = ReturnZero,
    .get_sset_count = GetSSetCount,
    .get_ts_info    = ethtool_op_get_ts_info,
    .nway_reset     = NWayReset,
    .set_msglevel   = SetMsgLevel,
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

    aThis->mPciDev = aPciDev;

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

void Adapter_Destroy(Adapter* aThis)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    unregister_netdev(aThis->mNetDev);
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

// ===== Entry points - EthTool =============================================

uint32_t GetMsgLevel(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    Adapter* lAdapter = netdev_priv(aNetDev);

    return lAdapter->mMsgLevel;
}

int GetSSetCount(struct net_device* aNetDev, int aSSet)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    int lResult = - EOPNOTSUPP;

    switch (aSSet)
    {
    case ETH_SS_TEST : lResult = 0; break;
    case ETH_SS_STATS: lResult = 0; break;
    }
    
    return lResult;
}

int NWayReset(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    // NOTE  Restart autonegotiation

    return 0;
}

int ReturnZero(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    return 0;
}

void SetMsgLevel(struct net_device* aNetDev, uint32_t aValue)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    Adapter* lAdapter = netdev_priv(aNetDev);

    lAdapter->mMsgLevel = aValue;
}

// ===== Entry points - Net Device ==========================================

int EthIoCtl(struct net_device* aNetDev, struct ifreq* aRequest, int aCmd)
{
    printk(KERN_DEBUG PREFIX "%s( , ,  )\n", __FUNCTION__);
    
    return - EOPNOTSUPP;
}

int Open(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
    
    return 0;
}

int SetFeatures(struct net_device* aNetDev, netdev_features_t aFeatures)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
    
    return 0;
}

int SetMacAddress(struct net_device* aNetDev, void* aAddr)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    int lResult = - EADDRNOTAVAIL;

    struct sockaddr* lAddr = aAddr;

	if (is_valid_ether_addr(lAddr->sa_data))
    {
		eth_hw_addr_set(aNetDev, lAddr->sa_data);

        lResult = 0;
    }

    return lResult;
}

void SetRxMode(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);
}

int Stop(struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s(  )\n", __FUNCTION__);

    return 0;
}

void TxTimeout(struct net_device* aNetDev, unsigned int aTxQueue)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);
}

netdev_tx_t StartXmit(struct sk_buff* aBuffer, struct net_device* aNetDev)
{
    printk(KERN_DEBUG PREFIX "%s( ,  )\n", __FUNCTION__);

    return NETDEV_TX_OK;
}

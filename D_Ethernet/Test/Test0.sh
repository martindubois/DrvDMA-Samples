#!/bin/sh

# Author    KMS - Martin Dubois, P. Eng.
# Copyright (C) 2026 KMS
# License   http://www.apache.org/licenses/LICENSE-2.0
# Product   DrvDMS-Sample
# File      D_Ethernet/Test/Test0.sh

echo Executing  Test.sh  ...

test_exist () {
    echo - $1

    if [ ! -e $1 ] ; then
        echo ERROR  $1  does not exist
    fi
}

test_cat () {
    test_exist $1

    cat $1
    echo
}

test_ls () {
    test_exist $1

    ls -l $1
    echo
}

echo ----- 0. /dev ----------------------------------------------------------

echo 0.a /dev/DrvDMA0

test_ls /dev/DrvDMA0
read -p "INSTRUCTION Verify the dev node is listed, note the major number and press ENTER" RESPONSE

echo ----- 1. /sys ----------------------------------------------------------

echo --- 1.a /sys/bus/pci/drivers/D_Ethernet ---

test_ls /sys/bus/pci/drivers/D_Ethernet
read -p "INSTRUCTION Verify the link to PCI device is present and press ENTER" RESPONSE

test_exist /sys/bus/pci/drivers/D_Ethernet/bind
test_exist /sys/bus/pci/drivers/D_Ethernet/module
test_exist /sys/bus/pci/drivers/D_Ethernet/new_id
test_exist /sys/bus/pci/drivers/D_Ethernet/remove_id
test_exist /sys/bus/pci/drivers/D_Ethernet/uevent
test_exist /sys/bus/pci/drivers/D_Ethernet/unbind

echo --- 1.b /sys/class/D_Ethernet ---

test_exist /sys/class/D_Ethernet

test_ls /sys/class/D_Ethernet/DrvDMA0
read -p "INSTRUCTION Verify the link to virtual device is present and press ENTER" RESPONSE

echo --- 1.c /sys/class/net ---

test_ls /sys/class/net

test_exist /sys/class/net/eth0

echo --- 1.d /sys/devices/virtual/D_Ethernet ---

test_exist /sys/devices/virtual/D_Ethernet
test_exist /sys/devices/virtual/D_Ethernet/DrvDMA0

test_cat /sys/devices/virtual/D_Ethernet/DrvDMA0/dev
read -p "INSTRUCTION Verify the major number and press ENTER" RESPONSE

test_ls /sys/devices/virtual/D_Ethernet/DrvDMA0/power

test_ls /sys/devices/virtual/D_Ethernet/DrvDMA0/subsystem
read -p "INSTRUCTION Verify the link to the class is present and press ENTER" RESPONSE

echo --- 1.e /sys/devices/virtual/net ---

test_ls /sys/devices/virtual/net

test_exist /sys/devices/virtual/net/eth0

test_cat /sys/devices/virtual/net/eth0/addr_assign_type

test_cat /sys/devices/virtual/net/eth0/address
read -p "INSTRUCTION Verify the Ethernet address (04:05:06:07:08:09) and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/addr_len
read -p "INSTRUCTION Verify the Ethernet address length (6) and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/broadcast
read -p "INSTRUCTION Verify the broadcast address (ff:ff:ff:ff:ff:ff) and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/carrier
test_cat /sys/devices/virtual/net/eth0/carrier_changes
test_cat /sys/devices/virtual/net/eth0/carrier_down_count
test_cat /sys/devices/virtual/net/eth0/carrier_up_count
test_cat /sys/devices/virtual/net/eth0/dev_id
test_cat /sys/devices/virtual/net/eth0/dev_port
test_cat /sys/devices/virtual/net/eth0/dormant

test_exist /sys/devices/virtual/net/eth0/duplex

test_cat /sys/devices/virtual/net/eth0/flags
test_cat /sys/devices/virtual/net/eth0/gro_flush_timeout
test_cat /sys/devices/virtual/net/eth0/ifalias
test_cat /sys/devices/virtual/net/eth0/ifindex
read -p "INSTRUCTION Note the interface index and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/iflink
read -p "INSTRUCTION Verify the interface index and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/link_mode
test_cat /sys/devices/virtual/net/eth0/mtu
read -p "INSTRUCTION Verify the MTU (1500) and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/name_assign_type
test_cat /sys/devices/virtual/net/eth0/napi_defer_hard_irqs
test_cat /sys/devices/virtual/net/eth0/netdev_group
test_cat /sys/devices/virtual/net/eth0/operstate

test_exist /sys/devices/virtual/net/eth0/phys_port_id
test_exist /sys/devices/virtual/net/eth0/phys_port_name
test_exist /sys/devices/virtual/net/eth0/phys_switch_id

test_ls /sys/devices/virtual/net/eth0/power

test_cat /sys/devices/virtual/net/eth0/proto_down

test_exist /sys/devices/virtual/net/eth0/queues
test_exist /sys/devices/virtual/net/eth0/queues/rx-0

test_cat /sys/devices/virtual/net/eth0/queues/rx-0/rps_cpus
test_cat /sys/devices/virtual/net/eth0/queues/rx-0/rps_flow_cnt

test_exist /sys/devices/virtual/net/eth0/queues/tx-0
test_exist /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits

test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/hold_time
read -p "INSTRUCTION Verify the hold time (1000) and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/inflight
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/limit
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/limit_max
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/limit_min
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/stall_cnt
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/stall_max
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/byte_queue_limits/stall_thrs

test_exist /sys/devices/virtual/net/eth0/queues/tx-0/traffic_class

test_cat /sys/devices/virtual/net/eth0/queues/tx-0/tx_maxrate
test_cat /sys/devices/virtual/net/eth0/queues/tx-0/tx_timeout

test_exist /sys/devices/virtual/net/eth0/queues/tx-0/xps_cpus

test_cat /sys/devices/virtual/net/eth0/queues/tx-0/xps_rxqs

test_exist /sys/devices/virtual/net/eth0/speed

test_ls /sys/devices/virtual/net/eth0/subsystem
read -p "INSTRUCTION Verify the link to the class is present and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/testing
test_cat /sys/devices/virtual/net/eth0/threaded
test_cat /sys/devices/virtual/net/eth0/tx_queue_len
read -p "INSTRUCTION Verify the tx queue length (1000) and press ENTER" RESPONSE

test_cat /sys/devices/virtual/net/eth0/type
test_cat /sys/devices/virtual/net/eth0/uevent
read -p "INSTRUCTION Verify the interface name (eth0) and the interface index and press ENTER" RESPONSE

echo --- 1.f /sys/module/D_Ethernet ---

test_exist /sys/module/D_Ethernet

test_cat /sys/module/D_Ethernet/coresize

test_exist /sys/module/D_Ethernet/drivers

test_ls /sys/module/D_Ethernet/drivers/pci:D_Ethernet

test_exist /sys/module/D_Ethernet/holders

test_cat /sys/module/D_Ethernet/initsize 
test_cat /sys/module/D_Ethernet/initstate
read -p "INSTRUCTION Verify the init state (live) and press ENTER" RESPONSE

test_exist /sys/module/D_Ethernet/notes
test_exist /sys/module/D_Ethernet/notes/.note.gnu.build-id

test_cat /sys/module/D_Ethernet/notes/.note.Linux
read -p "INSTRUCTION Verify the note (LinuxLinux) and press ENTER" RESPONSE

test_cat /sys/module/D_Ethernet/refcnt
read -p "INSTRUCTION Verify the reference counter (0) and press ENTER" RESPONSE

test_ls /sys/module/D_Ethernet/sections

test_cat /sys/module/D_Ethernet/srcversion
test_cat /sys/module/D_Ethernet/taint

test_exist /sys/module/D_Ethernet/uevent

echo ----- 2. ethtool -------------------------------------------------------

sudo ethtool eth0

read -p "INSTRUCTION Verify the message level (0) and the link detection (yes) and press ENTER" RESPONSE

echo ----- 3. ip ------------------------------------------------------------

ip -c -d -s link

read -p "INSTRUCTION For the eth0 link, verify the MTU (1500), the Ethernet address (04:05:06:07:08:09) and the broadcast address (ff:ff:ff:ff:ff:ff), and press ENTER" RESPONSE

# ===== End =====
echo OK

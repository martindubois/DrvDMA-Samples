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

echo 0. /dev/DrvDMA0

test_ls /dev/DrvDMA0

echo 1. /sys/bus/pci/drivers/D_Ethernet

test_ls /sys/bus/pci/drivers/D_Ethernet

test_exist /sys/bus/pci/drivers/D_Ethernet/bind
test_exist /sys/bus/pci/drivers/D_Ethernet/module
test_exist /sys/bus/pci/drivers/D_Ethernet/new_id
test_exist /sys/bus/pci/drivers/D_Ethernet/remove_id
test_exist /sys/bus/pci/drivers/D_Ethernet/uevent
test_exist /sys/bus/pci/drivers/D_Ethernet/unbind

echo 2. /sys/class/D_Ethernet

test_exist /sys/class/D_Ethernet

test_ls /sys/class/D_Ethernet/DrvDMA0

echo 3. /sys/class/net

test_ls /sys/class/net

echo 4. /sys/devices/virtual/D_Ethernet

test_exist /sys/devices/virtual/D_Ethernet
test_exist /sys/devices/virtual/D_Ethernet/DrvDMA0

test_cat /sys/devices/virtual/D_Ethernet/DrvDMA/dev

test_ls /sys/devices/virtual/D_Ethernet/DrvDMA/power
test_ls /sys/devices/virtual/D_Ethernet/DrvDMA/subsystem

echo 5. /sys/devices/virtual/net

test_ls /sys/devices/virtual/net

test_exist /sys/devices/virtual/net/eth0

test_cat /sys/devices/virtual/net/eth0/addr_assign_type
test_cat /sys/devices/virtual/net/eth0/address

echo 6. /sys/module/D_Ethernet

test_exist /sys/module/D_Ethernet

test_cat /sys/module/D_Ethernet/coresize

test_exist /sys/module/D_Ethernet/drivers

test_ls /sys/module/D_Ethernet/drivers/pci:D_Ethernet

test_exist /sys/module/D_Ethernet/holders

test_cat /sys/module/D_Ethernet/initsize 
test_cat /sys/module/D_Ethernet/initstate

test_exist /sys/module/D_Ethernet/notes
test_exist /sys/module/D_Ethernet/notes/.note.gnu.build-id

test_cat /sys/module/D_Ethernet/notes/.note.Linux
test_cat /sys/module/D_Ethernet/refcnt

test_ls /sys/module/D_Ethernet/sections

test_cat /sys/module/D_Ethernet/srcversion
test_cat /sys/module/D_Ethernet/taint

test_exist /sys/module/D_Ethernet/uevent

# ===== End =====
echo OK

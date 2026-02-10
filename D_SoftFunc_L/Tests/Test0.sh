#!/bin/sh

# Author    KMS - Martin Dubois, P. Eng.
# Copyright (C) 2026 KMS
# License   http://www.apache.org/licenses/LICENSE-2.0
# Product   DrvDMA-Sample
# File      D_Ethernet/Tests/Test0.sh

echo Executing  Test.sh  ...

# ===== Functions ===========================================================

test_exist () {
    echo - $1

    if [ ! -e $1 ] ; then
        echo ERROR  $1  does not exist
        read -p "Press ENTER to continue\n" RESPONSE
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

# ===== Execution ===========================================================

echo ----- 1. /sys ----------------------------------------------------------

echo --- 1.a /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L ---

test_exist /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L
test_exist /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L/bind
test_exist /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L/DrvDMA.toto.0
test_exist /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L/module
test_exist /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L/uevent
test_exist /sys/bus/auxiliary/drivers/D_SoftFunc_L.D_SoftFunc_L/unbind

echo --- 1.b /sys/devices/virtual/DrvDMA/DrvDMA0/DrvDMA.toto.0

test_exist /sys/devices/virtual/DrvDMA/DrvDMA0/DrvDMA.toto.0/driver

echo --- 1.c /sys/module/D_SoftFunc_L ---

test_exist /sys/module/D_SoftFunc_L

test_cat /sys/module/D_SoftFunc_L/coresize

test_exist /sys/module/D_SoftFunc_L/drivers
test_exist /sys/module/D_SoftFunc_L/drivers/auxiliary:D_SoftFunc_L.D_SoftFunc_L
test_exist /sys/module/D_SoftFunc_L/holders

test_cat /sys/module/D_SoftFunc_L/initsize

test_cat /sys/module/D_SoftFunc_L/initstate
read -p "INSTRUCTION Verify the init state (live) and press ENTER" RESPONSE

test_exist /sys/module/D_SoftFunc_L/notes
test_exist /sys/module/D_SoftFunc_L/notes/.note.gnu.build-id

test_cat /sys/module/D_SoftFunc_L/notes/.note.Linux
read -p "INSTRUCTION Verify the note (LinuxLinux) and press ENTER" RESPONSE

test_cat /sys/module/D_SoftFunc_L/refcnt

test_ls /sys/module/D_SoftFunc_L/sections

test_cat /sys/module/D_SoftFunc_L/srcversion
test_cat /sys/module/D_SoftFunc_L/taint

test_exist /sys/module/D_SoftFunc_L/uevent

# ===== End =================================================================
echo OK


    Author    KMS - Martin Dubois, P. Eng.
    Copyright (C) 2024-2026 KMS
    License   http://www.apache.org/licenses/LICENSE-2.0
    Product   DrvDMA-Samples
    File      README.md

    ===== Before compiling ==================================================

Before compiling the DrvDMA samples, you must install DrvDMA 3.0.17-beta.

Before compiling the D_Ethernet samples on Linux, you must copy DrvDMA_Glue.c
from the source folder (usr/src/DrvDMA-3.0) to the sample folder.

    ===== Samples summary ===================================================

    D_Ethernet

This sample is a very simple Linux NIC driver using DrvDMA library.

    D_NDIS

This sample is a very simple Windows NIC driver using DrvDMA library.

    U_BAR

This sample is a very simple program using DrvDMA library and DrvDMA driver
to access registers of a PCIe device.

    U_Simple

This sample is a very simple program using DrvDMA library you can run without
installing the DrvDMA driver.

    U_Test

This sample is a test program you can use to test the DrvDMA driver
installation and configuration.

    U_XDMA_C2H

This sample configure a C2H XDMA channel and transfer data.

    U_XDMA_H2C

This sample configure a H2C XDMA channel and transfer data.

    U_XDMA_H2C_Perf

This sample configure a H2C XDMA channel and transfer data measuing
performance.

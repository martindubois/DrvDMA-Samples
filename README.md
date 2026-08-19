
    Author    KMS - Martin Dubois, P. Eng.
    Copyright (C) 2024-2026 KMS
    License   http://www.apache.org/licenses/LICENSE-2.0
    Product   DrvDMA-Samples
    File      README.md

    ===== Before compiling ==================================================

Before compiling the DrvDMA samples, you must install DrvDMA 3.0.17-beta.

Before compiling the D_Ethernet or D_SoftFunc_L samples on Linux, you must
copy DrvDMA_Glue.c from the source folder (var/lib/dkms/DrvDMA/3.0/source) to
the sample folder.

    ===== Samples summary ===================================================

    D_Ethernet - Linux - No DMA engine used

This sample is a very simple Linux NIC driver using DrvDMA library.

    D_NDIS - Windows - No DMA engine used

This sample is a very simple Windows NIC driver using DrvDMA library and the
NetAdapterCx framework.

    D_SoftFunc_L - Linux - No DMA engine used

This sample is an empty software function driver on Linux.

    U_BAR - Windows - No DMA engine used

This sample is a very simple program using DrvDMA library and DrvDMA driver
to access registers of a PCIe device.

    U_Int - Windows

This sample measures the delay between an interrupt and the call of the user
mode callback.

    U_Simple - Linux and Windows - No DMA engine used

This sample is a very simple program using DrvDMA library you can run without
installing the DrvDMA driver.

    U_Test - Linux and Windows

This sample is a test program you can use to test the DrvDMA driver
installation and configuration.

    U_XDMA_C2H - Windows - AMD (Xilinx) XDMA

This sample configure a C2H XDMA channel and transfer data.

    U_XDMA_H2C - Windows - AMD (Xilinx) XDMA

This sample configure a H2C XDMA channel and transfer data.

    U_XDMA_H2C_Perf - Windows - AMD (Xilinx) XDMA

This sample configure a H2C XDMA channel and transfer data measuing
performance.

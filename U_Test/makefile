
# Author    KMS - Martin Dubois, P. Eng.
# Copyright (C) 2025 KMS
# License   http://www.apache.org/licenses/LICENSE-2.0
# Product   DrvDMA-Samples
# File      U_Test/makefile

OUTPUT = ../Binaries/U_Test.exe

SOURCES = U_Test.cpp

INCLUDES = -I /usr/local/DrvDMA-3.0/inc

LIBRARIES = /usr/local/DrvDMA-3.0/lib/DrvDMA_U.a /usr/local/DrvDMA-3.0/lib/KMS-C.a /usr/local/DrvDMA-3.0/lib/KMS-A.a

CFLAGS = @../Config.args

# ===== Rules ===============================================================

.cpp.o:
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets =============================================================

$(OUTPUT) : $(OBJECTS)
	g++ $ -o $@ $^ $(LIBRARIES)

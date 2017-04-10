This repository contains source code that can be conpiled with NXP LPCXpresso v8.2.2
The workspace must contain project CMSIS_CORE_LPC17xx and SE2_Lib before compile any project.

Project SE2_Lib contains two possible build configurations one for LPCXpresso LPC1769 board and NGX Blueboard LPC1768-H
one of this configurations must be selected before building the library

New projects must also have the same build configurations where LPCXpresso defines the symbol __LPCX__ and Blueboard defines __BB__, also must reference project CMSIS_CORE_LPC17xx and SE2_Lib, correspondent inc and lib directories must be configured also root includ dir .


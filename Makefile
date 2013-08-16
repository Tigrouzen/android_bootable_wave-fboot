OutDir := out
ObjDir := out
SrcDir := src

SrcMake := src/Makefile

ifdef SystemRoot
	RM := del /Q
	FixPath = $(subst /,\,$1)
	SYS := WIN
else
	ifeq ($(shell uname), Linux)
		RM := rm -f
		FixPath = $1
		SYS := LINUX
	endif
endif
CROSS_COMPILE = C:\bada\2.0.6\Tools\Toolchains\ARM\bin

CC := $(CROSS_COMPILE)\arm-bada-eabi-gcc.exe
AS := $(CROSS_COMPILE)\arm-bada-eabi-as.exe
LD := $(CROSS_COMPILE)\arm-bada-eabi-ld.exe
OBJCOPY := $(CROSS_COMPILE)\arm-bada-eabi-objcopy

Applications := FBOOT_S8500_b2x
OutputFiles := $(patsubst %, $(OutDir)/%.fota, $(Applications))

application: $(OutputFiles)

rebuild: clean application

include $(SrcMake)

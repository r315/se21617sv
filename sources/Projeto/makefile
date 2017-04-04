CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
DBG = arm-none-eabi-insight
GDB = arm-none-eabi-gdb
AR = arm-none-eabi-ar
OBJDUMP = arm-none-eabi-objdump

AFLAGS= -g
CFLAGS= -g -mapcs
LFLAGS= 
LDSCRIPT= -T ../../bsp/ldscript
LDSCRIPT_RAM= -T ../../bsp/ldscript_ram


TARGET = project
LIBTARGET =

OBJS = init.o $(TARGET).o system.o save.o idle.o config.o space.o sprites/sprites.o

SYMBOLS =
LIBS = -lled -ltimer -lbutton -lrtc -lclock -lspi -llcd -lflash -lgcc -lc
LIBSPATH = -L../../libs -L/usr/local/arm-tools/arm-2009q1/lib/gcc/arm-none-eabi/4.3.3 -L/usr/local/arm-tools/arm-2009q1/arm-none-eabi/lib
INCSPATH = -I../../includes

all: $(TARGET).axf

flash: $(OBJS)
	$(LD) $(LFLAGS) $(LDSCRIPT) $(LIBSPATH) $(OBJS) -o $(TARGET).axf $(LIBS)

$(TARGET).axf: $(OBJS)
	$(LD) $(LFLAGS) $(LDSCRIPT_RAM) $(LIBSPATH) $(OBJS) -o $(TARGET).axf $(LIBS)

init.o:
	$(AS) $(AFLAGS) ../../bsp/init.S -o init.o

clean:
	rm -f $(OBJS) $(XOBJS) $(TARGET).axf $(TARGET) $(TARGET).exe
	
debug:  $(TARGET).axf
	@$(DBG) $(TARGET).axf

dump: $(TARGET).axf
	$(OBJDUMP) -d -S $(TARGET).o > $(TARGET).lst

symbols:
	$(OBJDUMP) -t $(TARGET).axf

lst:
	$(OBJDUMP) -S --disassemble $(TARGET).axf > $(TARGET).lst

.c.o:
	$(CC) $(CFLAGS) $(SYMBOLS)  $(INCSPATH) -c $< -o $(<:.c=.o)
###############################################################
UNAME_S := $(shell uname -s)
GPP = gcc

VPATH = $(dir $(XSRCS))

XSRCPATH =src
XOBJPATH =Debug/src
XINCPATH =../../include ../emulator inc
XLIBPATH =../emulator

#XSRCS =$(foreach sdir,$(XSRCPATH),$(wildcard $(sdir)/*.c))
XSRCSF =config.c idle.c Projeto.c save.c system.c space.c rand.c
XSRCS =$(addprefix $(XSRCPATH)/, $(XSRCSF)) sprites/sprites.c 
XOBJS = $(addprefix $(XOBJPATH)/,$(notdir $(XSRCS:.c=.obj)))

#XOBJS=$(TARGET).obj system.obj save.obj idle.obj config.obj space.obj sprites/sprites.obj

ifeq ($(UNAME_S),Linux)

XLIBS =-lemu -lSDL2
XCFLAGS =-Wall -D__EMU__ -DLCD_LANDSCAPE

emulator: $(TARGET)
#	@echo $(XSRCS)
#$(TARGET)
	$(XOBJPATH)/$(TARGET)

else
DRIVE =C
COMPILER_FLAGS = -D_EMU_ -w -Wl,-subsystem,windows
INCLUDES += -I$(DRIVE):/tools/SDL2/include -I$(DRIVE):/tools/SDL2/include -I../../include -I../emulator
LINKER_FLAGS = -L$(DRIVE):/tools/SDL2/lib -L$(DRIVE):/tools/MinGw/lib -L../emulator -lemu -lmingw32 -lSDL2main -lSDL2

$(TARGET).exe: $(XOBJS)
	$(GPP) $(XOBJS) $(LINKER_FLAGS) -o $(TARGET)

%.obj : %.c
	$(GPP) $(INCSPATH) -Wall -D__EMU__ -c $< -o $(<:.c=.obj)
	
emulator: $(TARGET).exe
	cmd /c $(TARGET).exe
endif

$(TARGET): $(XOBJS)
	$(GPP)  $(XOBJS) $(XCFLAGS) $(addprefix -L, $(XLIBPATH)) $(XLIBS) -o $(XOBJPATH)/$(TARGET)

$(XOBJPATH)/%.obj : %.c
	$(GPP) -D__EMU__ $(addprefix -I, $(XINCPATH)) $(XCFLAGS) -c $< -o $@
	

	



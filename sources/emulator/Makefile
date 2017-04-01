#OBJS specifies which files to compile as part of the project 
SRCS=lcdsdlTest.c
SRCS += lcdsdl.c
SRCS += emulator.c
SRCS += lcd.c
SRCS += font8x16.c
INCLUDES=-I"../../include"

TARGET = lcdsdltest

OBJS= $(SRCS:.c=.o)

CC = gcc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
COMPILER_FLAGS = -D_EMU_ -Wall
LINKER_FLAGS = -lSDL2
else
COMPILER_FLAGS = -D_EMU_ -w -Wl,-subsystem,windows
INCLUDES += -I"C:/tools/SDL2/include" -I"C:/tools/SDL2/include"
LINKER_FLAGS = -L"C:/tools/SDL2/lib" -LC:/tools/MinGw/lib -lmingw32 -lSDL2main -lSDL2
endif

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LINKER_FLAGS) -o $(TARGET)
	
run: $(TARGET)
	./$(TARGET)		

clean:
	rm -f $(OBJS) $(TARGET)

.c.o:
	$(CC) -c $< $(INCLUDES) $(COMPILER_FLAGS) -o $(<:.c=.o)
	
#####################################################
LIB_OBJS = lcdsdl.o lcd.o emulator.o font8x16.o
lib: $(LIB_OBJS)
#	$(GCC)  $(INCSPATH) -Wall -lSDL2 -c $(EDIR)/lcdsdl.c -o lcdsdl.o
#	$(GCC)  $(INCSPATH) -Wall -lSDL2 -c $(EDIR)/lcd.c -o lcd.o
#	$(GCC)  $(INCSPATH) -Wall -lSDL2 -c $(EDIR)/emulator.c -o emulator.o
#	$(GCC)  $(INCSPATH) -Wall -lSDL2 -c $(EDIR)/font8x16.c -o font8x16.o
	rm -f libemu.a
	ar rcs libemu.a lcdsdl.o lcd.o emulator.o font8x16.o
	rm -f *.o

	

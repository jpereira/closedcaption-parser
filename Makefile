LIBNAME := libclosedcaption.so
CC := $(CC)
CFLAGS += -W -Wall -I. -Iinclude/

CFLAGS += -DDEBUG_PACKET_PES
# CFLAGS += -DDEBUG_CAPTION_DATA
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_SOUND
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_BITMAP
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_GEOMETRIC
# CFLAGS += -DDEBUG_CAPTION_MANAGEMENT
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_1BYTEDRCS
# CFLAGS += -DDEBUG_CAPTION_CLOSED_CONTROLCODE

LIB_OBJS = \
	src/caption-closed-controls.o       \
	src/caption-closed.o                \
	src/caption-common.o                \
	src/caption-data.o                  \
	src/caption-hooking.o               \
	src/caption-management.o            \
	src/caption-superimpose-1bytedrcs.o \
	src/caption-superimpose-2bytedrcs.o \
	src/caption-superimpose-bitmap.o    \
	src/caption-superimpose-colormap.o  \
	src/caption-superimpose-geometric.o \
	src/caption-superimpose-sound.o     \
	src/caption-superimpose.o           \
	src/packet-pes.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

all: $(LIB_OBJS)
	$(CC) -shared -o $(LIBNAME) $(LIB_OBJS)

clean:
	rm -f $(LIBNAME) $(LIB_OBJS)

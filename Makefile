CC := gcc
CFLAGS += -W -Wall -I. -Iinclude/
DEPS = 

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

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< 

all: $(LIB_OBJS)
	$(CC) -shared -o libclosedcaption.so $(LIB_OBJS)

clean:
	rm -f $(LIB_OBJS)

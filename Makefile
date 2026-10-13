LIBNAME := libclosedcaption.so
CFLAGS += -W -Wall -fPIC -I. -Iinclude/

# Gera os arquivos .d, para recompilar quando um header mudar.
CFLAGS += -MMD -MP

# Compiladores fora do Xcode (ex: llvm do Homebrew) não acham o SDK sozinhos
ifeq ($(shell uname -s),Darwin)
SDKROOT ?= $(shell xcrun --show-sdk-path)
CFLAGS += -isysroot $(SDKROOT)
LDFLAGS += -isysroot $(SDKROOT) -L$(SDKROOT)/usr/lib
endif

# Depuração: DEBUG_CLOSEDCAPTION liga as mensagens (ATV_DEBUG/ATV_HEXDUMP) no
# stdout. As demais flags adicionam dumps de cada módulo e só têm efeito junto
# com ela.
# CFLAGS += -DDEBUG_CLOSEDCAPTION
# CFLAGS += -DDEBUG_PACKET_PES
# CFLAGS += -DDEBUG_CAPTION_DATA
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_SOUND
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_BITMAP
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_GEOMETRIC
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_COLORMAP
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_1BYTEDRCS
# CFLAGS += -DDEBUG_CAPTION_SUPERIMPOSE_2BYTEDRCS

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
	src/packet-pes.o                    \
	src/caption-screen.o                \
	src/ts-reader.o                     \
	src/ts-demux.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(LIB_OBJS)
	$(CC) $(LDFLAGS) -shared -o $(LIBNAME) $(LIB_OBJS)

EXAMPLE := example/example

example: $(EXAMPLE)

$(EXAMPLE): example/main.o $(LIB_OBJS)
	$(CC) $(LDFLAGS) -o $@ example/main.o $(LIB_OBJS) $(LDLIBS)

SAMPLE_TS := docs/tv-camara-isdbtb-legenda.ts
SAMPLE_PID := 0x03ed

# Executa o exemplo com a amostra: primeiro lista os PIDs, depois exibe as legendas.
run-example: $(EXAMPLE)
	./$(EXAMPLE) $(SAMPLE_TS)
	@echo
	./$(EXAMPLE) $(SAMPLE_TS) $(SAMPLE_PID)

# Testes: roda o exemplo com a amostra real e com os TS sintéticos de tests/
# (gerados por tests/gerar-sinteticos.py), comparando com a saída esperada.
test: $(EXAMPLE)
	@mkdir -p build/test
	./$(EXAMPLE) $(SAMPLE_TS) > build/test/tv-camara-pids.txt
	diff -u tests/tv-camara-pids.expected build/test/tv-camara-pids.txt
	./$(EXAMPLE) $(SAMPLE_TS) $(SAMPLE_PID) 2>/dev/null > build/test/tv-camara-legendas.txt
	diff -u tests/tv-camara-legendas.expected build/test/tv-camara-legendas.txt
	./$(EXAMPLE) tests/sintetico-basico.ts 0x130 2>/dev/null > build/test/sintetico-basico.txt
	diff -u tests/sintetico-basico.expected build/test/sintetico-basico.txt
	./$(EXAMPLE) tests/sintetico-casos-limite.ts 0x130 2>/dev/null > build/test/sintetico-casos-limite.txt
	diff -u tests/sintetico-casos-limite.expected build/test/sintetico-casos-limite.txt
	@echo "OK: todas as saídas iguais às esperadas."

# Documentação (Doxygen): build/docs/html/index.html
docs:
	mkdir -p build/docs
	doxygen Doxyfile

clean:
	rm -f $(LIBNAME) $(LIB_OBJS) $(LIB_OBJS:.o=.d) $(EXAMPLE) example/main.o example/main.d
	rm -rf build

-include $(LIB_OBJS:.o=.d) example/main.d

.PHONY: all example run-example test docs clean

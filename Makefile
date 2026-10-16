LIBNAME := libclosedcaption.so
STATICLIB := libclosedcaption.a

# A versão vem do arquivo VERSION (o bump.sh mexe nele)
VERSION := $(shell cat VERSION)
MAJOR := $(word 1,$(subst ., ,$(VERSION)))

PREFIX ?= /usr/local
DESTDIR ?=

CFLAGS += -W -Wall -fPIC -I. -Iinclude/ -Iinclude/closedcaption/ -DCLOSEDCAPTION_VERSION=\"$(VERSION)\"

# Gera os arquivos .d, para recompilar quando um header mudar.
CFLAGS += -MMD -MP

# Compiladores fora do Xcode (ex: llvm do Homebrew) não acham o SDK sozinhos
ifeq ($(shell uname -s),Darwin)
SOEXT = dylib
SOREAL = libclosedcaption.$(VERSION).dylib
SOMAJOR = libclosedcaption.$(MAJOR).dylib
SOLINK = libclosedcaption.dylib
SOFLAGS = -install_name $(PREFIX)/lib/$(SOMAJOR)
else
SOEXT = so
SOREAL = libclosedcaption.so.$(VERSION)
SOMAJOR = libclosedcaption.so.$(MAJOR)
SOLINK = libclosedcaption.so
SOFLAGS = -Wl,-soname,$(SOMAJOR)
endif

ifeq ($(shell uname -s),Darwin)
SDKROOT ?= $(shell xcrun --show-sdk-path)
CFLAGS += -isysroot $(SDKROOT)
LDFLAGS += -isysroot $(SDKROOT) -L$(SDKROOT)/usr/lib
endif

# Depuração: DEBUG_CLOSEDCAPTION liga as mensagens (DTV_DEBUG/DTV_HEXDUMP) no
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
	src/ts-demux.o                      \
	src/caption-version.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(LIB_OBJS)
	$(CC) $(LDFLAGS) -shared $(SOFLAGS) -o $(LIBNAME) $(LIB_OBJS)
	$(AR) rcs $(STATICLIB) $(LIB_OBJS)

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
	$(MAKE) --no-print-directory test-api

# Testes da API (versão, erros, TS/PES quebrado)
TEST_API := build/test-api

test-api: $(LIB_OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TEST_API) tests/test-api.c $(LIB_OBJS) $(LDLIBS)
	./$(TEST_API)

# No macOS o ASan não tem detector de leak (e o ASAN_OPTIONS do ambiente pode derrubar o teste)
ASAN_ENV = ASAN_OPTIONS=detect_leaks=$(if $(filter Darwin,$(shell uname -s)),0,1)

# Mesmos testes com AddressSanitizer + UBSan (usa objetos separados em build/san)
test-sanitize:
	@rm -rf build/san && mkdir -p build/san
	$(CC) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
	  -o build/san/test-api tests/test-api.c $(LIB_OBJS:.o=.c) $(LDFLAGS) $(LDLIBS)
	$(ASAN_ENV) build/san/test-api
	$(CC) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
	  -o build/san/example example/main.c $(LIB_OBJS:.o=.c) $(LDFLAGS) $(LDLIBS)
	$(ASAN_ENV) build/san/example $(SAMPLE_TS) $(SAMPLE_PID) >/dev/null 2>&1
	$(ASAN_ENV) build/san/example tests/sintetico-casos-limite.ts 0x130 >/dev/null 2>&1
	@echo "OK: sanitizers sem erro."

# Fuzz com libFuzzer (precisa de clang): make fuzz && build/fuzz-pes -max_total_time=60
fuzz:
	@mkdir -p build
	clang $(CFLAGS) -O1 -g -fsanitize=fuzzer,address,undefined \
	  -o build/fuzz-pes tests/fuzz/fuzz-pes.c $(LIB_OBJS:.o=.c) $(LDFLAGS)

# Análise estática
lint:
	cppcheck --error-exitcode=1 --enable=warning,portability -q -I include/closedcaption src example

# libclosedcaption.pc
libclosedcaption.pc: libclosedcaption.pc.in VERSION
	sed -e 's|@PREFIX@|$(PREFIX)|' -e 's|@VERSION@|$(VERSION)|' $< > $@

install: all libclosedcaption.pc
	install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig $(DESTDIR)$(PREFIX)/include/closedcaption
	install -m 0755 $(LIBNAME) $(DESTDIR)$(PREFIX)/lib/$(SOREAL)
	ln -sf $(SOREAL) $(DESTDIR)$(PREFIX)/lib/$(SOMAJOR)
	ln -sf $(SOMAJOR) $(DESTDIR)$(PREFIX)/lib/$(SOLINK)
	install -m 0644 $(STATICLIB) $(DESTDIR)$(PREFIX)/lib/
	install -m 0644 libclosedcaption.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig/
	install -m 0644 include/closedcaption/*.h $(DESTDIR)$(PREFIX)/include/closedcaption/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/$(SOREAL) $(DESTDIR)$(PREFIX)/lib/$(SOMAJOR) \
	  $(DESTDIR)$(PREFIX)/lib/$(SOLINK) $(DESTDIR)$(PREFIX)/lib/$(STATICLIB) \
	  $(DESTDIR)$(PREFIX)/lib/pkgconfig/libclosedcaption.pc
	rm -rf $(DESTDIR)$(PREFIX)/include/closedcaption

# Tarball da release (git archive): build/libclosedcaption-X.Y.Z.tar.gz
dist:
	@mkdir -p build
	git archive --format=tar.gz --prefix=libclosedcaption-$(VERSION)/ \
	  -o build/libclosedcaption-$(VERSION).tar.gz HEAD
	cd build && shasum -a 256 libclosedcaption-$(VERSION).tar.gz > libclosedcaption-$(VERSION).tar.gz.sha256

# Documentação (Doxygen): build/docs/html/index.html
docs:
	mkdir -p build/docs
	(cat Doxyfile; echo 'PROJECT_NUMBER = $(VERSION)') | doxygen -

clean:
	rm -f $(LIBNAME) $(STATICLIB) libclosedcaption.pc $(LIB_OBJS) $(LIB_OBJS:.o=.d) $(EXAMPLE) example/main.o example/main.d
	rm -rf build

-include $(LIB_OBJS:.o=.d) example/main.d

.PHONY: all example run-example test test-api test-sanitize fuzz lint install uninstall dist docs clean

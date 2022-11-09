# for all the non-Mac systems (as well as for CLI Xcode-less Mac builds)

CC ?= cc
CFLAGS := -Itinyhttp -I. -Wall -Wextra -Werror -std=c99 $(CFLAGS)
ifndef RELEASE
CFLAGS := $(CFLAGS) -g -DDEBUG=1
endif

LD = $(CC)

TARGETS = hash.o \
	  intarray.o \
	  privutil.o \
	  tcp.o \
	  main.o
TARGET = srv

all: $(TARGET)

$(TARGET): $(TARGETS)
	$(LD) -o $(TARGET) $(LDFLAGS) $(TARGETS) $(LIBS)

$(TARGETS):
	$(CC) -c -o $@ $(CFLAGS) tinyhttp/$(shell basename $@ .o).c


.PHONY: all clean distclean

clean: distclean

distclean:
	-rm -rf *.dSYM $(TARGET) $(TARGETS)

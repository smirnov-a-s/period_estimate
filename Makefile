SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)
TARGET = main

CC = gcc
CPPFLAGS =
CFLAGS   = -MMD -MP -Wall -g -std=gnu99

LD = gcc
LDFLAGS = -I/usr/lib/i386-linux-gnu/
# LDLIBS  = -lm -lfftw3f
LDLIBS  = -lm -lfftw3f

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:%.c=%.d)
endif

clean:
	rm $(OBJECTS) $(TARGET) *.d

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)
TARGET = main

CC = gcc
# CFLAGS   = -MMD -MP -Wall -g -std=gnu99
# mac os x
CFLAGS = -MMD -MP -Wall -g -std=gnu99 -I/usr/local/Cellar/fftw/3.3.4_1/include

LD = gcc
# LDFLAGS = -I/usr/lib/i386-linux-gnu/
# mac os x
LDFLAGS = -L/usr/local/Cellar/fftw/3.3.4_1/lib
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

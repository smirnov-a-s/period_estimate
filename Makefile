SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)
TARGET = main

CC = gcc
CFLAGS = -MMD -MP -Wall -g -std=gnu99

LD = gcc
LDFLAGS =
LDLIBS = -lm

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

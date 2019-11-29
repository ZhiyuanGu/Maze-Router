# This is the makefile for ECE1387 Assignment 1 Maze Router
# Developer: 	Zhiyuan Gu (10004920400)
# Date:		Sep 24, 2019

CC := g++
RM := rm -f

CFLAGS := -Wall
LDFLAGS :=
LDLIBS := -lX11

OBJFILES := main.o Routing.o RoutePath.o track.o graphics.o draw.o
TARGET := MR

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	$(RM) $(OBJFILES) $(TARGET)

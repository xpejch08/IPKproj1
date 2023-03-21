CC = g++
CFLAGS = -Wall -Werror
TARGET = main.cpp

all:
	g++ main.cpp -o ipkpd
clean:
	$(RM) ipkpd

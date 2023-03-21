CC = g++
CFLAGS = -Wall -Werror
TARGET = main.cpp

all:
	g++ -std=c++20 main.cpp -o ipkpd
clean:
	$(RM) ipkpd

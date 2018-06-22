# =============================================
# make 設置
# =============================================
CC=g++
CFLAGS:= -O3
CFLAGS+= -std=c++14
CFLAGS+= -Wall
CFLAGS+= -fopenmp

INCPATH:=
INCPATH+= -I./OpenBMP

# =============================================
# 編譯檔案命令 [tag:cmd]
# =============================================
# 連結所有編譯後的檔案產生可執行檔
main.exe: OpenBMP_main.o
	$(CC) $(CFLAGS) *.o -o main.exe

# 編譯所有cpp檔案
OpenBMP_main.o: OpenBMP_main.cpp
	$(CC) $(CFLAGS) $(INCPATH) -c OpenBMP_main.cpp

# =============================================
# 命令列
# =============================================
all: build
build: main.exe
rebuild: clean build
clear:
	rm -f *.o main.exe
run: build
	./main.exe
rerun: clear run
# =============================================
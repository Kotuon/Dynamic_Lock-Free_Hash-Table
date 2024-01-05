PRG=gnu.exe

GCC=g++
GCCFLAGS=-O3 -Wall -Wextra -std=c++11 -latomic -g

VALGRIND_OPTIONS=-q --leak-check=full --track-origins=yes
DIFFLAGS=--strip-trailing-cr -y --suppress-common-lines

OBJECTS0=
DRIVER0=driver.cpp

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	clear
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) -pthread
0 1 2 3 4 5 6 7 8 9 10 11:
	@echo "running test$@"
	./$(PRG) $@
mem0 mem1 mem2 mem3 mem4 mem5 mem6 mem7 mem8 mem9 mem10 mem11:
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@)
clean:
	rm -f *.exe *.o *.obj
run:
	./$(PRG)
run-mem:
	valgrind $(VALGRIND_OPTIONS) ./$(PRG)
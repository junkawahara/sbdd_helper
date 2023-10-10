SBDDDIR = ../../SAPPOROBDD
SBDDSRCDIR = $(SBDDDIR)/src
SBDDINCLUDEDIR = $(SBDDDIR)/include
OPT = -O3 -DB_64 -I$(SBDDINCLUDEDIR)
WOPT = -Wall -Wextra -Wformat=2
OLDGCC = /usr/local/gcc-4.1.2/bin/gcc
OLDGPP = /usr/local/gcc-4.1.2/bin/g++
CLANG = clang
CLANGPP = clang++

# http://codeforces.com/blog/entry/15547  -Wshift-overflow=2 -Wduplicated-cond 
WOPT2 = -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal \
-Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC \
-D_FORTIFY_SOURCE=2 -fstack-protector
#-fsanitize=address -fsanitize=undefined \
#-fno-sanitize-recover
# -pedantic-errors

WOPT3 = $(WOPT2) -Wlogical-op
WOPT4 = $(WOPT3) -Wconversion

all: testc testcpp testcpp-gmp
	./testc
	./testcpp
	./testcpp-gmp

oldgcc: testoldgcc testoldgpp
	./testoldgcc
	./testoldgpp

clang: testclang testcppclang
	./testclang
	./testcppclang

testc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h bddc.o
	gcc $(OPT) $(WOPT4) -Wc++-compat bddc.o SBDD_helper_testc.c -o testc

testoldgcc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h bddc-oldgcc.o
	$(OLDGCC) $(OPT) $(WOPT2) -Wno-long-long -Wc++-compat bddc-oldgcc.o SBDD_helper_testc.c -o testoldgcc

testclang: SBDD_helper_testc_cpp.h SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h bddc-clang.o
	$(CLANG) $(OPT) $(WOPT2) -Wc++-compat bddc-clang.o SBDD_helper_testc.c -o testclang

testcpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h bddc.o BDD.o ZBDD.o
	g++ $(OPT) $(WOPT4) bddc.o BDD.o ZBDD.o SBDD_helper_testcpp.cpp -o testcpp

testoldgpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h bddc-oldgcc.o BDD-oldgpp.o ZBDD-oldgpp.o
	$(OLDGPP) $(OPT) $(WOPT2) -Wno-long-long bddc-oldgcc.o BDD-oldgpp.o ZBDD-oldgpp.o SBDD_helper_testcpp.cpp -o testoldgpp

testcppg: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h bddc.o BDD.o ZBDD.o
	g++ -g -DB_64 $(WOPT4) bddc.o BDD.o ZBDD.o SBDD_helper_testcpp.cpp -o testcppg

testcpp-gmp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h bddc.o BDD.o ZBDD.o
	g++ $(OPT) $(WOPT4) -DUSE_GMP bddc.o BDD.o ZBDD.o SBDD_helper_testcpp.cpp -lgmp -lgmpxx -o testcpp-gmp

testcpp98: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h bddc.o BDD.o ZBDD.o
	g++ -std=c++98 -Wno-long-long $(OPT) $(WOPT4) bddc.o BDD.o ZBDD.o SBDD_helper_testcpp.cpp -o testcpp98

testcppclang: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h bddc-clang.o BDD-clang.o ZBDD-clang.o
	$(CLANGPP) $(OPT) $(WOPT2) bddc-clang.o BDD-clang.o ZBDD-clang.o SBDD_helper_testcpp.cpp -o testcppclang

testnewc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h bddc.o
	gcc $(OPT) $(WOPT4) -DSBDD_VERSION=185 bddc.o SBDD_helper_testc.c -o testc

testnewcpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h bddc.o BDD.o ZBDD.o
	g++ $(OPT) $(WOPT4) -DSBDD_VERSION=185 bddc.o BDD.o ZBDD.o SBDD_helper_testcpp.cpp -o testcpp

bddc.o: $(SBDDSRCDIR)/BDDc/bddc.c $(SBDDINCLUDEDIR)/bddc.h
	gcc $(OPT) $(SBDDSRCDIR)/BDDc/bddc.c -c -o bddc.o

bddc-oldgcc.o: $(SBDDSRCDIR)/BDDc/bddc.c $(SBDDINCLUDEDIR)/bddc.h
	$(OLDGCC) $(OPT) $(SBDDSRCDIR)/BDDc/bddc.c -c -o bddc-oldgcc.o

bddc-clang.o: $(SBDDSRCDIR)/BDDc/bddc.c $(SBDDINCLUDEDIR)/bddc.h
	$(CLANG) $(OPT) $(SBDDSRCDIR)/BDDc/bddc.c -c -o bddc-clang.o

BDD.o: $(SBDDSRCDIR)/BDD+/BDD.cc $(SBDDINCLUDEDIR)/BDD.h
	g++ $(OPT) $(SBDDSRCDIR)/BDD+/BDD.cc -c -o BDD.o

BDD-oldgpp.o: $(SBDDSRCDIR)/BDD+/BDD.cc $(SBDDINCLUDEDIR)/BDD.h
	$(OLDGPP) $(OPT) $(SBDDSRCDIR)/BDD+/BDD.cc -c -o BDD-oldgpp.o

BDD-clang.o: $(SBDDSRCDIR)/BDD+/BDD.cc $(SBDDINCLUDEDIR)/BDD.h
	$(CLANGPP) $(OPT) $(SBDDSRCDIR)/BDD+/BDD.cc -c -o BDD-clang.o

ZBDD.o: $(SBDDSRCDIR)/BDD+/ZBDD.cc $(SBDDINCLUDEDIR)/ZBDD.h
	g++ $(OPT) $(SBDDSRCDIR)/BDD+/ZBDD.cc -c -o ZBDD.o

ZBDD-oldgpp.o: $(SBDDSRCDIR)/BDD+/ZBDD.cc $(SBDDINCLUDEDIR)/ZBDD.h
	$(OLDGPP) $(OPT) $(SBDDSRCDIR)/BDD+/ZBDD.cc -c -o ZBDD-oldgpp.o

ZBDD-clang.o: $(SBDDSRCDIR)/BDD+/ZBDD.cc $(SBDDINCLUDEDIR)/ZBDD.h
	$(CLANGPP) $(OPT) $(SBDDSRCDIR)/BDD+/ZBDD.cc -c -o ZBDD-clang.o

clean:
	rm -f *.o *.stackdump testc.exe testc testcpp.exe testcpp testoldgcc.exe testoldgcc testoldgpp.exe testoldgpp testcpp98.exe testcppg.exe testcppg testcpp98 testcpp-gmp.exe testcpp-gmp testcppclang

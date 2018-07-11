SBDDDIR = ../SBDD
OPT = -O3 -DB_64
WOPT = -Wall -Wextra -Wformat=2

# http://codeforces.com/blog/entry/15547  -Wshift-overflow=2 -Wduplicated-cond 
WOPT2 = -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal \
-Wconversion -Wlogical-op \
-Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC \
-D_FORTIFY_SOURCE=2 -fstack-protector
#-fsanitize=address -fsanitize=undefined \
#-fno-sanitize-recover
# -pedantic-errors

all: testc testcpp
	./testc
	./testcpp

testc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h $(SBDDDIR)/bddc.o
	gcc $(OPT) $(WOPT2) -Wc++-compat $(SBDDDIR)/bddc.o SBDD_helper_testc.c -o testc

testcpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h  ../devel/*.h $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o
	g++ $(OPT) $(WOPT2) $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o SBDD_helper_testcpp.cpp -o testcpp

testnewc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h $(SBDDDIR)/bddc.o
	gcc $(OPT) $(WOPT2) -DSBDD_VERSION=185 $(SBDDDIR)/bddc.o SBDD_helper_testc.c -o testc

testnewcpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c ../devel/SBDD_helper.h ../devel/*.h $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o
	g++ $(OPT) $(WOPT2) -DSBDD_VERSION=185 $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o SBDD_helper_testcpp.cpp -o testcpp

$(SBDDDIR)/bddc.o: $(SBDDDIR)/bddc.c $(SBDDDIR)/bddc.h
	gcc $(OPT) $(SBDDDIR)/bddc.c -c -o $(SBDDDIR)/bddc.o

$(SBDDDIR)/BDD.o: $(SBDDDIR)/BDD.cc $(SBDDDIR)/BDD.h
	g++ $(OPT) $(SBDDDIR)/BDD.cc -c -o $(SBDDDIR)/BDD.o

$(SBDDDIR)/ZBDD.o: $(SBDDDIR)/ZBDD.cc $(SBDDDIR)/ZBDD.h
	g++ $(OPT) $(SBDDDIR)/ZBDD.cc -c -o $(SBDDDIR)/ZBDD.o

clean:
	rm -f $(SBDDDIR)/*.o testc.exe testc testcpp.exe testcpp

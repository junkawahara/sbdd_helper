SBDDDIR = SBDD
OPT = -O3 -DB_64
WOPT = -Wall -Wextra -Wformat=2 -pedantic-errors

all: testc testcpp

testc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c SBDD_helper.h $(SBDDDIR)/bddc.o
	gcc $(OPT) $(WOPT) -Wc++-compat $(SBDDDIR)/bddc.o SBDD_helper_testc.c -o testc

testcpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c SBDD_helper.h $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o
	g++ $(OPT) $(WOPT) $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o SBDD_helper_testcpp.cpp -o testcpp

testnewc: SBDD_helper_testc_cpp.h SBDD_helper_testc.c SBDD_helper.h $(SBDDDIR)/bddc.o
	gcc $(OPT) $(WOPT) -DSBDD_VERSION=185 $(SBDDDIR)/bddc.o SBDD_helper_testc.c -o testc

testnewcpp: SBDD_helper_testc_cpp.h SBDD_helper_testcpp.cpp SBDD_helper_testc.c SBDD_helper.h $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o
	g++ $(OPT) $(WOPT) -DSBDD_VERSION=185 $(SBDDDIR)/bddc.o $(SBDDDIR)/BDD.o $(SBDDDIR)/ZBDD.o SBDD_helper_testcpp.cpp -o testcpp

$(SBDDDIR)/bddc.o: $(SBDDDIR)/bddc.c $(SBDDDIR)/bddc.h
	gcc $(OPT) $(SBDDDIR)/bddc.c -c -o $(SBDDDIR)/bddc.o

$(SBDDDIR)/BDD.o: $(SBDDDIR)/BDD.cc $(SBDDDIR)/BDD.h
	g++ $(OPT) $(SBDDDIR)/BDD.cc -c -o $(SBDDDIR)/BDD.o

$(SBDDDIR)/ZBDD.o: $(SBDDDIR)/ZBDD.cc $(SBDDDIR)/ZBDD.h
	g++ $(OPT) $(SBDDDIR)/ZBDD.cc -c -o $(SBDDDIR)/ZBDD.o

clean:
	rm -f $(SBDDDIR)/*.o testc.exe testc testcpp.exe testcpp

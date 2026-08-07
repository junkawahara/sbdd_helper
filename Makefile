# The build and test rules are in tests/Makefile. This Makefile only
# forwards the targets to it so that make can also be run at the
# repository root.
#
# The location of the SAPPOROBDD source tree (which must have the src/
# and include/ directories) is specified by SBDDDIR. Its default value
# is ../../SAPPOROBDD as seen from tests/, that is, the SAPPOROBDD
# directory placed next to this repository. Note that a relative path
# given here is interpreted from tests/, and thus specifying an
# absolute path is recommended:
#
#     make SBDDDIR=/path/to/SAPPOROBDD all
#
# The distributed header SBDD_helper.h is not built by this Makefile.
# It is generated from the sources in devel/ by
#
#     cd devel && python3 combine.py

TESTDIR = tests

TARGETS = all clang oldgcc \
testc testc99 testcpp testcpp-gmp testcpp98 \
testclang testcppclang testoldgcc testoldgpp \
testnewc testnewcpp clean

.PHONY: $(TARGETS)

$(TARGETS):
	$(MAKE) -C $(TESTDIR) $@

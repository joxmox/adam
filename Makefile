
# config
gtest		= $(HOME)/Tools/googletest-release-1.7.0

# flags
std			= -std=c++14
cxx_opts	= $(std) -g3 -fmax-errors=1 -Wall -Wno-sign-compare
app_incl	= -I$(dom) -I$(utl)
app_libs	= -L$(lib) -l$(libName) -llog4cxx -lpthread -lncurses
tst_incl	= -I$(gtest)/include
tst_libs	= -L$(gtest)/lib -lgtest -lgtest_main -lpthread

# programs
cc = g++
rm = rm -rf
ar = ar crs
md = mkdir -p

# commands
compile	= $(cc) $(cxx_opts) $(app_incl) -c $< -o $@
link 	= $(cc) $(std) $+ -o $@ $(app_libs)
doxy	= doxygen
web		= firefox
run 	= $<

# dirs
src = src/
app = $(src)app/
cfg = $(src)conf/
dom = $(src)domain/
tst = $(src)test/
utl = $(src)util/
sh  = $(src)scripts/

bld	   = build/
bin    = $(bld)bin/
lib    = $(bld)libs/
obj    = $(bld)objs/
dist   = $(bld)dist/
objApp = $(obj)app/
objLib = $(obj)lib/
objTst = $(obj)tst/
doc    = $(bld)docs/
docApi = $(doc)api/
docTst = $(doc)test/

# names
libName  = adam
libFile  = lib${libName}.a

distName = mt_dlc_input
distFile = ${distName}.tgz


# vars
tstNames	:= $(basename $(notdir $(wildcard $(tst)*.cpp)))
tstObjs		:= $(addsuffix .o,$(addprefix $(objTst),$(tstNames)))


###########################################################
# top-level targets
help :
	@echo "Top-level targets:"
	@echo "make clean    - removes the $(bld) dir"
	@echo "make all      - performs a clean build, run the tests and generate the docs"
	@echo "make dist     - copies all application files into $(dist)"
	@echo "make tar      - creates a TAR file of the application distribution into $(bld)"
	@echo "make app      - build the applications into $(bin)"
	@echo "make doc      - generate the program docs into $(doc). append 'doc_show' to view the report in firefox"
	@echo "make test     - run the unit tests and generates a test report into $(rep). append 'test_show' to view the report in firefox"
	@echo "make lib      - build the lib file into $(lib)"


clean :
	$(rm) $(bld)*

app  : lib app_compile
test : lib test_compile test_run test_report
doc  : doc_generate
lib  : dirs lib_compile lib_assemble
dist : app dist_dirs copy_apps copy_data copy_scripts copy_conf
tar  : dist dist_tar
all  : clean tar test doc


###########################################################
# -----------------------
# init
# -----------------------

dirs : 
	$(md) $(objApp) $(objLib) $(objTst) $(bin) $(lib) $(docApi) $(docTst)


# -----------------------
# lib
# -----------------------

lib_compile : util_compile domain_compile

lib_assemble :
	$(md) $(lib)
	$(ar) $(lib)$(libFile) $(objLib)*.o

util_compile : $(objLib)stringUtils.o $(objLib)Curse.o 

domain_compile : $(objLib)Buffer.o $(objLib)Editor.o


# -----------------------
# lib objs
# -----------------------


$(objLib)stringUtils.o : $(utl)stringUtils.cpp $(utl)stringUtils.hpp
	$(compile)
	
$(objLib)Curse.o : $(utl)Curse.cpp $(utl)Curse.hpp 
	$(compile)
	
$(objLib)Buffer.o : $(dom)Buffer.cpp $(dom)Buffer.hpp 
	$(compile)

$(objLib)Editor.o : $(dom)Editor.cpp $(dom)Editor.hpp $(dom)Buffer.hpp $(utl)Curse.hpp
	$(compile)


# -----------------------
# app
# -----------------------
app_compile : lib $(bin)adam $(bin)ct

# -----------------------
# app exes
# -----------------------
$(bin)adam : $(objApp)adam.o $(lib)$(libFile)  
	$(link)
	
$(bin)ct : $(objApp)ct.o $(lib)$(libFile)  
	$(link)


# -----------------------
# app objs
# -----------------------
$(objApp)adam.o : $(app)adam.cpp $(dom)Editor.hpp
	$(compile)

$(objApp)ct.o : $(app)ct.cpp $(utl)Curse.hpp
	$(compile)


# -----------------------
# test
# -----------------------
test_compile : lib $(bin)unitTest

test_run : 
	-$(bin)unitTest --gtest_output=xml:$(docTst)gtest-results.xml

test_report :
	@if [ -e $(docTst)gtest-results.xml ] && `which groovy > /dev/null`; then \
		groovy $(cfg)test-report.groovy -dir=$(docTst) < $(docTst)gtest-results.xml 2> /dev/null; \
	else \
		echo "*** cannot generate HTML test report. please, install groovy"; \
	fi

test_show : 
	@if [ -e $(docTst)index.html ]; then \
		$(web) $(doc)test/index.html; \
	fi


$(bin)unitTest : $(tstObjs) $(objLib)stringUtils.o
	$(link) $(tst_libs)

	
$(objTst)stringUtilsTest.o : $(tst)stringUtilsTest.cpp  $(utl)stringUtils.hpp $(utl)stringUtils.cpp
	$(compile) $(tst_incl)



# -----------------------
# docs
# -----------------------
doc_generate : 
	$(md) $(doc)api
	$(doxy) $(cfg)doxy.conf

doc_show : 
	$(web) $(doc)api/html/index.html


# -----------------------
# dist
# -----------------------
dist_dirs:
	${md} ${dist}bin 

copy_apps:
#	cp $(bin)dlcGenerator ${dist}bin
#	cp $(bin)dlcProcessor ${dist}bin
#	cp $(bin)dumpDlc      ${dist}bin/dlcDump
#	cp $(bin)umtfDump     ${dist}bin
#	cp $(bin)reportTicker ${dist}bin

copy_data:
#	cp data/gen/* ${dist}generator
 
copy_scripts:
#	cp ${sh}clean.sh ${sh}dlcgen.sh ${sh}setup.sh ${dist}
#	for f in `ls ${dist}*.sh | cat`; do \
#		chmod +x $$f; \
#	done

copy_conf:
#	cp ${cfg}log4cxx.conf ${dist}conf

dist_tar:
#	tar cvfz ${bld}${distFile} -C ${dist} .



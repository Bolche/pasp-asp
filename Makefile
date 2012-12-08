#General definitions
TARGET=pasp-asp
BUILD_DIR=build
BIN_DIR=bin
MKDIR=mkdir
CP=cp
RM=rm
CC=g++
EXTRA_LDFLAGS=

#File definitions
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
OBJECTS_NOMAIN = $(OBJECTS:$(BUILD_DIR)/main.o=)
TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(addprefix $(BUILD_DIR)/, $(TEST_SOURCES:.cpp=.o))

#Define type of compilation
TYPE=release
ifeq ($(TYPE),debug)
    LDFLAGS = -lm $(EXTRA_LDFLAGS)
    CCFLAGS = -Wall -pedantic -I. -g -O0
    PRINTDEBUG = 1
endif

ifeq ($(TYPE),release)
    LDFLAGS = -s -pipe -lm $(EXTRA_LDFLAGS)
    CCFLAGS = -Wall -pedantic -I. -O3 -march=native
    PRINTDEBUG = 0
endif

ifeq ($(PRINTDEBUG),1)
    CCFLAGS += -DPRINT_DEBUG
endif

#Test for C++11 support
TESTSOURCEFILE := $(shell mktemp XXXXXXXX.cpp)
TEMP := $(shell printf '\#include <list>\n\#include "c++11_warning.h"\nint main(){std::list<int> a={1,2,3};\nfor(auto i: a){};\nreturn 0;\n}' > $(TESTSOURCEFILE) )
TEMP := $(shell $(CC) $(TESTSOURCEFILE) -I. -o cpp11test 2> /dev/null) #try compiling without flags
ifneq ($(shell test -f cpp11test || echo 0),0)
	CPP11FLAG = 
else
	TEMP := $(shell $(CC) $(TESTSOURCEFILE) -I. -std=c++11 -o cpp11test 2> /dev/null) #try compiling with c++11 flag
	ifneq ($(shell test -f cpp11test || echo 0),0)
		CPP11FLAG = -std=c++11
	else
		TEMP := $(shell $(CC) $(TESTSOURCEFILE) -I. -std=c++0x -o cpp11test 2> /dev/null) #try compiling with c++0x flag
		ifneq ($(shell test -f cpp11test || echo 0),0)
			CPP11FLAG = -std=c++0x
		else
			CPP11FLAG = 0
		endif
	endif
endif
TEMP := $(shell $(RM) -f $(TESTSOURCEFILE) cpp11test)
ifeq ($(CPP11FLAG), 0)
        $(error This program requires C++11 support and the compiler does not provide it. Use a newer compiler)
else
        CCFLAGS += $(CPP11FLAG)
endif

# Begin rules
.PHONY: clean dirs clean-deps

all: dirs $(BIN_DIR)/$(TARGET)
	@printf "[*] Build complete\n"

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CC) -o $(BIN_DIR)/$(TARGET) $(LDFLAGS) $(CCFLAGS) $(OBJECTS)

test: dirs $(BIN_DIR)/test
	@printf "[*] Executing tests\n"
	@$(BIN_DIR)/test

$(BIN_DIR)/test: $(OBJECTS) $(TEST_OBJECTS)
	$(CC) -o $@ $(CCFLAGS) $(LDFLAGS) $(TEST_OBJECTS) $(OBJECTS_NOMAIN) -lcppunit

dirs:
	@$(MKDIR) -pv $(BUILD_DIR)
	@$(MKDIR) -pv $(BUILD_DIR)/tests
	@$(MKDIR) -pv $(BIN_DIR)

clean:
	@printf "[*] Removing object files\n"
	@$(RM) -frv $(BUILD_DIR) $(BIN_DIR)
	@$(RM) -fv Makefile.deps
	@$(RM) -fv *~
	@$(RM) -fv tests/*~

clean-deps:
	@$(RM) -fv Makefile.deps

Makefile.deps:
	@printf "[*] Computing build dependencies\n"
	@printf "# File auto-generated by Makefile. DO NOT EDIT\n\n" > Makefile.deps
	@for FILE in $(SOURCES); do \
		printf "\$$(BUILD_DIR)/" >> Makefile.deps; \
		$(CC) $(CCFLAGS) $$FILE -MM  >> Makefile.deps; \
		printf "\t\$$(CC) -c -o \$$@ \$$< \$$(CCFLAGS)\n" >> Makefile.deps ; \
	done
	@for FILE in $(TEST_SOURCES); do \
		printf "\$$(BUILD_DIR)/tests/" >> Makefile.deps; \
		$(CC) $(CCFLAGS) $$FILE -MM >> Makefile.deps; \
		printf "\t\$$(CC) -c -o \$$@ \$$< \$$(CCFLAGS)\n" >> Makefile.deps; \
	done

#This file is autogenerated if necessary and contains the rules for compiling each source file
include Makefile.deps

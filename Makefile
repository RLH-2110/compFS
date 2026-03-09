
CC=gcc
CCFLAGS = -ansi -pedantic -Wall

OUTPUT = libcmpFS.a
CFILES= $(wildcard src/*.c) 
OFILES := $(patsubst src/%.c,obj/%.o,$(CFILES))
CHEADERS = $(wildcard src/*.h)

TESTER_OUTPUT = test/testFS.exe
TEST_CFILES := $(CFILES) test/tests.c test/req/str.c
TEST_CHEADERS = $(CHEADERS) test/tests.h test/req/str.h

# OS FLAGS
ifeq ($(OS),Windows_NT)
    CCFLAGS += -D OS_WINDOWS
$(info    detected windows)
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
$(info    detected Linux)
	CCFLAGS += -D OS_LINUX
	endif

	ifeq ($(UNAME_S),Haiku)
$(info    detected Haiku)
    CCFLAGS += -D OS_HAIKU
	endif

endif

ifeq ($(OS),Windows_NT)
	MKDIR = cmd /C mkdir
else
	MKDIR = mkdir
endif

# rules

$(OUTPUT): $(OFILES)
	ar rcs $(OUTPUT) $(OFILES)

obj/%.o: src/%.c $(CHEADERS) obj
	$(CC) -c $< $(CCFLAGS) -o $@

obj:
	$(MKDIR) obj

test: $(OUTPUT)
	$(CC) -o $(TESTER_OUTPUT) $(TEST_CFILES) $(CCFLAGS) -L. -lcmpFS
	./$(TESTER_OUTPUT)
	
	
# clean
clear: clean
clean:
	
ifeq ($(OS),Windows_NT)
	rm $(OUTPUT).exe
else
	rm $(OUTPUT)
endif
	rm $(TESTER_OUTPUT)
	rm log.txt out.txt src.txt 


	
.PHONY : clean clear test

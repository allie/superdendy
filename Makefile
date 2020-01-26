CC=clang++
CFLAGS=-std=c++17 -g -c -Wno-nullability-completeness
INC=-Iinclude -I/usr/local/include -Ilib/inih/
LDFLAGS=-lSDL2 -lSDL2_image -lm
SOURCES=$(shell find src -name "*.cc" -not -name "*.partial.cc")
OBJDIR=build
OBJECTS=$(SOURCES:%.cc=$(OBJDIR)/%.o)
EXECUTABLE=superdendy
OS=$(shell g++ -dumpmachine)

ifneq (, $(findstring mingw, $(OS)))
	LDFLAGS := -lmingw32 -lSDL2main $(LDFLAGS)
	CC=g++
endif

all: $(SOURCES) $(EXECUTABLE)

debug: CFLAGS += -DDEBUG_MODE
debug: all

nestest: CFLAGS += -DNESTEST
nestest: all

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJDIR)/%.o: %.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -rf $(OBJDIR)

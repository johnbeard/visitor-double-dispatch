SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
	DEPS=$(SOURCES:.cpp=.d)
BINS=$(SOURCES:.cpp=)

CFLAGS+=-MMD
CXXFLAGS+=-MMD

all: $(BINS)

.PHONY: clean

clean:
	$(RM) $(OBJECTS) $(DEPS) $(BINS)

-include $(DEPS)

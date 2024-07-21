CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++14 -O3
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O3

# SystemC paths
SYSTEMC_HOME := $(PWD)/lib/systemc
SYSTEMC_INC := $(SYSTEMC_HOME)/include
SYSTEMC_LIB := $(SYSTEMC_HOME)/lib

SYSTEMC_LIB_FILE := $(SYSTEMC_LIB)/libsystemc.so

INCLUDES := -Isrc -I$(SYSTEMC_INC)
LDFLAGS := -L$(SYSTEMC_LIB) -Wl,-rpath,$(SYSTEMC_LIB) $(SYSTEMC_LIB_FILE)
CPPFLAGS := -DSC_INCLUDE_FX -DSC_CPLUSPLUS=201402L

SRCS := src/csv_parser.c src/arg_parser.c src/main.c
CPPSRCS := src/impl/simulation.cpp
OBJS := $(SRCS:.c=.o) $(CPPSRCS:.cpp=.o)

TARGET := main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

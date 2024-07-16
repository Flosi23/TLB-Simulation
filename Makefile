CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++14 -g
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -g

# SystemC paths
SYSTEMC_SRC := $(PWD)/lib/systemc-src
SYSTEMC_BUILD := $(PWD)/lib/systemc-build
SYSTEMC_INSTALL := $(PWD)/lib/systemc-install
SYSTEMC_HOME := $(SYSTEMC_INSTALL)
SYSTEMC_INC := $(SYSTEMC_HOME)/include
SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-linux64

INCLUDES := -Isrc -I$(SYSTEMC_INC)
LDFLAGS := -L$(SYSTEMC_LIB) -lsystemc -Wl,-rpath,$(SYSTEMC_LIB)
CPPFLAGS := -DSC_INCLUDE_FX -DSC_CPLUSPLUS=201402L

SRCS := src/csv_parser.c src/arg_parser.c src/main.c
CPPSRCS := src/impl/simulation.cpp
OBJS := $(SRCS:.c=.o) $(CPPSRCS:.cpp=.o)

TARGET := main

.PHONY: all clean systemc

all: systemc $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) -v

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

systemc:
	@if [ ! -f $(SYSTEMC_LIB)/libsystemc.a ]; then \
		mkdir -p $(SYSTEMC_BUILD) && \
		cd $(SYSTEMC_BUILD) && \
		$(SYSTEMC_SRC)/configure --prefix=$(SYSTEMC_INSTALL) \
			CXX="$(CXX)" \
			CXXFLAGS="-O3 -std=c++14" \
			--disable-examples \
			--disable-tests && \
		$(MAKE) && \
		$(MAKE) install; \
	fi

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(SYSTEMC_BUILD) $(SYSTEMC_INSTALL)


CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++14 -O3
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O3

# SystemC paths
SYSTEMC_SRC := $(PWD)/lib/systemc-src
SYSTEMC_BUILD := $(PWD)/lib/systemc-build
SYSTEMC_INSTALL := $(PWD)/lib/systemc-install
SYSTEMC_HOME := $(SYSTEMC_INSTALL)
SYSTEMC_INC := $(SYSTEMC_HOME)/include

# Detect OS and architecture
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif
ARCH := $(shell uname -m)

# Set SystemC library path based on OS and architecture
ifeq ($(DETECTED_OS),Darwin)
    ifeq ($(ARCH),arm64)
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-macosxarm
    else
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-macosx64
    endif
else ifeq ($(DETECTED_OS),Linux)
    SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-linux64
else ifeq ($(DETECTED_OS),Windows)
    ifeq ($(ARCH),x86_64)
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-mingw64
    else
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-mingw32
    endif
else
    $(error Unsupported operating system: $(DETECTED_OS))
endif

SYSTEMC_LIB_FILE := $(SYSTEMC_LIB)/libsystemc.a
INCLUDES := -Isrc -I$(SYSTEMC_INC)
LDFLAGS := -L$(SYSTEMC_LIB) -Wl,-rpath,$(SYSTEMC_LIB) $(SYSTEMC_LIB_FILE)
CPPFLAGS := -DSC_INCLUDE_FX -DSC_CPLUSPLUS=201402L

SRCS := src/csv_parser.c src/arg_parser.c src/main.c
CPPSRCS := src/impl/simulation.cpp
OBJS := $(SRCS:.c=.o) $(CPPSRCS:.cpp=.o)
TARGET := main

.PHONY: all clean systemc

all: systemc $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

systemc:
	@if [ ! -f $(SYSTEMC_LIB_FILE) ]; then \
		echo "Building SystemC..." && \
		mkdir -p $(SYSTEMC_BUILD) && \
		cd $(SYSTEMC_BUILD) && \
		sed -i 's/aclocal-1\.14/aclocal/g' $(SYSTEMC_SRC)/configure && \
		$(SYSTEMC_SRC)/configure \
			--prefix=$(SYSTEMC_INSTALL) \
			CXX="$(CXX)" \
			CXXFLAGS="-O3 -std=c++14" \
			--disable-scv && \
		$(MAKE) && \
		$(MAKE) install && \
		echo "SystemC build completed."; \
	else \
		echo "SystemC is already built."; \
	fi

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(SYSTEMC_BUILD) $(SYSTEMC_INSTALL)

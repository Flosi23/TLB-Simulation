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

# Detect OS and architecture
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

ARCH := $(shell uname -m)

# Set SystemC library path based on OS and architecture
ifeq ($(DETECTED_OS),Darwin)
    SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-macosx$(shell if [ "$(ARCH)" = "x86_64" ]; then echo 64; fi)
else ifeq ($(DETECTED_OS),Linux)
    SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-linux$(shell if [ "$(ARCH)" = "x86_64" ]; then echo 64; fi)
else ifeq ($(DETECTED_OS),Windows)
    SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-mingw$(shell if [ "$(ARCH)" = "x86_64" ]; then echo 64; fi)
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
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) -v

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

systemc:
	@if [ ! -f $(SYSTEMC_LIB_FILE) ]; then \
		echo "Building SystemC..." && \
		mkdir -p $(SYSTEMC_BUILD) && \
		cd $(SYSTEMC_BUILD) && \
		echo "Configuring SystemC..." && \
		if timeout 300 $(SYSTEMC_SRC)/configure \
			--prefix=$(SYSTEMC_INSTALL) \
			CXX="$(CXX)" \
			CXXFLAGS="-O3 -std=c++14" \
			--enable-debug \
			--enable-verbose-config \
			--disable-scv \
			2>&1 | tee configure.log; then \
			echo "Compiling SystemC..." && \
			if $(MAKE) VERBOSE=1 -j$$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2) 2>&1 | tee make.log; then \
				echo "Installing SystemC..." && \
				if $(MAKE) install 2>&1 | tee install.log; then \
					echo "SystemC build completed."; \
				else \
					echo "SystemC installation failed. Check install.log for details."; \
					exit 1; \
				fi; \
			else \
				echo "SystemC compilation failed. Check make.log for details."; \
				exit 1; \
			fi; \
		else \
			echo "SystemC configuration failed or timed out. Check configure.log for details."; \
			exit 1; \
		fi; \
	else \
		echo "SystemC is already built."; \
	fi


clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(SYSTEMC_BUILD) $(SYSTEMC_INSTALL)
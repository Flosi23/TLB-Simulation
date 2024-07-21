# Compiler and flags
CXX := g++
CC := gcc
CXXFLAGS := -Wall -Wextra -std=c++14 -g
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

# Set SystemC library path and configuration options based on OS and architecture
ifeq ($(DETECTED_OS),Darwin)
    ifeq ($(ARCH),arm64)
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-macosxarm
        TARGET_ARCH := arm-apple-darwin
    else
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-macosx64
        TARGET_ARCH := macosx64
    endif
else ifeq ($(DETECTED_OS),Linux)
    ifeq ($(ARCH),$(filter $(ARCH),aarch64 arm64))
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-linux64
        TARGET_ARCH := linuxaarch64
    else ifeq ($(ARCH),x86_64)
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-linux64
        TARGET_ARCH := linux64
    else
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-linux32
        TARGET_ARCH := linux32
    endif
else ifeq ($(DETECTED_OS),Windows)
    ifeq ($(ARCH),x86_64)
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-mingw64
        TARGET_ARCH := mingw64
    else
        SYSTEMC_LIB := $(SYSTEMC_HOME)/lib-mingw32
        TARGET_ARCH := mingw32
    endif
else
    $(error Unsupported operating system: $(DETECTED_OS))
endif

SYSTEMC_LIB_FILE := $(SYSTEMC_LIB)/libsystemc.a

# Compiler and linker flags
INCLUDES := -Isrc -I$(SYSTEMC_INC)
LDFLAGS := -L$(SYSTEMC_LIB) -Wl,-rpath,$(SYSTEMC_LIB) -lsystemc
CPPFLAGS := -DSC_INCLUDE_FX -DSC_CPLUSPLUS=201402L

# Source files
SRCS := src/csv_parser.c src/arg_parser.c src/main.c
CPPSRCS := src/impl/simulation.cpp
OBJS := $(SRCS:.c=.o) $(CPPSRCS:.cpp=.o)

# Target executable
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
		if $(SYSTEMC_SRC)/configure \
			--prefix=$(SYSTEMC_INSTALL) \
			--target=$(TARGET_ARCH) \
			--enable-debug \
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
			echo "SystemC configuration failed. Check configure.log for details."; \
			exit 1; \
		fi; \
	else \
		echo "SystemC is already built."; \
	fi

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(SYSTEMC_BUILD) $(SYSTEMC_INSTALL)

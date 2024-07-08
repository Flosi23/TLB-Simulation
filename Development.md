## Disclaimer

```
As of now compiling has only been tested on MACOS. 
```

## Setup

It is recommended to use `CLION` as the IDE for this project as it makes working with `CMAKE` a lot easier.
It is not guaranteed that any of the other IDEs will work seamlessly with this project.

### Libs

The project uses the following libraries:

- systemc v.2.3.3

Systemc is already installed in ./lib/systemc. No further action has to be taken.

## Build and Execute

To build the project, run the following commands:

```bash
./build.sh
```

## Project Structure

The project is structured as follows:
This might be subject to change though!

```
.
├── build.sh
├── CMakeLists.txt
├── lib
│   └── systemc
│       ├── include
│       └── lib
│           └── cmake
│               └── SystemCLanguage
└── src
    ├── csv_parser.c
    ├── help.c
    ├── impl
    │   ├── simulation.cpp
    │   └── simulation.h
    ├── main.c
    └── types.h
```

- `build.sh`: This is a shell script used to build the project.
- `CMakeLists.txt`: This file is used by CMake to control the build process.
- `lib`: This directory contains the SystemC library.
- `src`: This directory contains the source code of the project
    - `csv_parser.c`: This file contains the implementation of the CSV parser.
    - `help.c`: This file contains the implementation of the help command.
    - `impl`: This directory contains the SystemC code for the actual TLB implementation
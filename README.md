# XPace Tools

Tools for working with log files of XPace enabled MRI sequences that store motion occurring during a scan.
The files are named `xpace_xxx.log` where `xxx` is a sequentially increasing number, the highest of which denotes 
the most recent scan.
XPace log files contain the motion the marker underwent during scanning in the scanner’s co-ordinate system.

This software is written in C++17, needs CMake version >= 3.15 and relies on the [Boost library](https://www.boost.org/) 
version >= 1.71.
The goal is to provide some simple tools like calculating statistics on motion tracking and to be the base for
visualization tools.

## Compilation

Clone the repository, make sure you have cmake version 3.15 or later and either open it in CLion. On the commandline,
follow this:

```shell script
cd XPaceTools
mkdir build
cd build

cmake -DBOOST_ROOT=/your/path/to/boost_1_71_0 ..
make
```
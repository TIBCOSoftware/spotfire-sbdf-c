# C Library for Spotfire® Binary Data Format (SBDF)

This library provides a C-language interface to read and write files in the
Spotfire Binary Data Format (SBDF).

### Installation

If you are using a development environment that supports directly opening
CMake projects, simply open this repository in that environment and build the
resulting project.

Otherwise, do the following:
1. [Download](https://cmake.org/download/) and install CMake for your system.
2. Open a shell or command prompt.  Make sure that your compiler toolchain is 
accessible on the `PATH` (i.e., without specifying the full path to the tools).
3. Optionally (but highly recommended), make a directory for building in and
change the working directory of the shell or command prompt to the new 
directory:
```sh
$ mkdir builddir
$ cd builddir
```
4. Run the `cmake` executable to generate the build project files used to build
the library.  You can include any 
[options](https://cmake.org/cmake/help/latest/manual/cmake.1.html#options)
supported by CMake (e.g., to specify a non-default generator).  If a build
directory was created in step 3 above, run: 
```sh
$ cmake path/to/spotfire-sbdf-c
```
Otherwise, run:
```sh
$ cmake .
```
5. Build the library using your compiler toolchain:
```sh
$ cmake --build .
```

### License
BSD-type 3-Clause License.  See the file `LICENSE` included in the repository.

# *vt* => virtual transport

## Introduction : What is *vt*?

*vt* is an active messaging layer that utilizes C++ object virtualization to
manage virtual endpoints with automatic location management. *vt* is directly
built on top of MPI to provide efficient portability across different machine
architectures. Empowered with virtualization, **vt** can automatically perform
dynamic load balancing to schedule scientific applications across diverse
platforms with minimal user input.

*vt* abstracts the concept of a `node`/`rank`/`worker`/`thread` so a program can
be written in terms of virtual entities that are location independent. Thus,
they can be automatically migrated and thereby executed on varying hardware
resources without explicit programmer mapping, location, and communication
management.

## Building

*vt* can be built with `cmake`.

To build *vt*, one must obtain the following dependencies:

### Optional:

#### If testing is enabled:
- Google Test (`gtest`), ([https://github.com/google/googletest.git](https://github.com/google/googletest.git))

#### If threading is enabled:
  - OpenMP       _or_
  - `std::thread`s (default from C++ compiler)

#### Required:
  - detector,   (*vt* ecosystem)
  - checkpoint, (*vt* ecosystem)
  - MPI,        (mpich/openmpi/mvapich)

### Automatic build

The "auto" builder is located here: [git@github.com:darma-mpi-backend/vt-auto-build.git]()

To learn how to use to auto builder follow the directions in the `vt-auto-build`
repository. The options to the script can be found by executing this on the
command line:

```bash
$ git clone git@github.com:darma-mpi-backend/vt-auto-build.git
$ cd vt-auto-build
$ perl ./auto.pl --help
  usage auto.pl <arguments>...
  Format each argument to script as: <argname>=<argval>
  Example: "auto.pl mode=fast"
    <argument-name>  |  <required> | <default>           |  <value>
    build_mode       |  true       | <no-default-value>  |  <no-default-value>
    compiler_c       |  true       | <no-default-value>  |  <no-default-value>
    compiler_cxx     |  true       | <no-default-value>  |  <no-default-value>
    vt_build_mode    |  false      |                     |
    root_dir         |  false      | <path>              |  <path>
    build_tests      |  false      | 1                   |  1
    kokkos           |  false      |                     |
    build_kokkos     |  false      | 0                   |  0
    prefix           |  false      |                     |
    par              |  false      | 14                  |  14
    dry_run          |  false      | 0                   |  0
    verbose          |  false      | 0                   |  0
    gtest            |  false      |                     |
    clean            |  false      | 0                   |  0
    backend          |  false      | 0                   |  0
    atomic           |  false      |                     |
    mpi_cc           |  false      |                     |
    mpi_cxx          |  false      |                     |
```
This command may get you most of the way there:

```bash
$ perl ./auto.pl build_mode=debug compiler_c=clang-3.9     \
  compiler_cxx=clang++-3.9 prefix=my-prefix-dir
```
###  Manual Build

To start out, obtain `gtest`, `detector`, and `checkpoint` (in that
order). Stitch them up with cmake, by passing the appropriate paths as they are
built.

Build and install them.

Once all the dependencies are properly installed, point *vt* cmake to the
install paths of each one of them. To build *vt*, the script located at
`vt/scripts/build_pl.pl` can be run to configure `cmake` for building. The
following is an example of how to use the script to configure with `openmpi` and
`clang 3.9` compilers:

```bash
$ mkdir vt-build
$ cd vt-build
$ perl ../vt/scripts/build_vt.pl build_mode=debug compiler=clang               \
  compiler_c=mpicc-openmpi-clang39 compiler_cxx=mpicxx-openmpi-clang39         \
  build_tests=1 vt_install=../vt-install
```

Run cmake and enjoy!

If *vt* is built with testing, `make test` can be executed to run the unit and
integration tests. The components `detector`, and `checkpoint` can be found
within the *vt* ecosystem. They are separate repositories but are exported as
separate cmake packages for reusability.


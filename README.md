# Libket

C++ library for quantum computation. See
https://quantum-ket.gitlab.io/libket.html for the API documentation.

## Run examples

Build requirements

* CMake 
* Ninja
* Conan 
* GMP

Runtime requirement

* Ket Bitwise simulator https://gitlab.com/quantum-ket/kbw

### Compile 

```console
$ git clone git@gitlab.com:quantum-ket/libket.git
$ cd libket
$ mkdir build
$ cd build
$ cmake .. -GNinja
$ ninja
```

### Execute 

Available examples:

* Quantum teleportation - [`teleport`](examples/teleport.cpp) 
* Random number generation - [`random`](examples/random.cpp) 

With [KBW](https://gitlab.com/quantum-ket/kbw#installation) running, execute:
```console
$ ./bin/<example>
```
> Replace `<example>` one of the available examples

### Ket program options

Every program linked with Libket accept the follow options 

```console
$ ./bin/teleport -h
Ket program options:
  -h [ --help ]              Show this information.
  -o [ --out ]               KQASM output file.
  -s [ --kbw ]  (=127.0.0.1) Quantum execution (KBW) address.
  -p [ --port ]  (=4242)     Quantum execution (KBW) port.
  --seed                     Set RNG seed for quantum execution.
  --no-execute               Does not execute KQASM, measurements return 0.
  --dump-to-fs               Use the filesystem to transfer dump data.
```

## Installation

```console
$ git clone git@gitlab.com:quantum-ket/libket.git
$ cd libket
$ mkdir build
$ cd build
$ cmake .. -GNinja
$ sudo ninja install
```

-----------

This project is part of the Ket Quantum Programming, see the documentation for
more information https://quantum-ket.gitlab.io.

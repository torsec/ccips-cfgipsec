# Install dependencies

 - I have cloned all the repositories in `/home/giacomo/Documents/programs/`
 - many git commands have been executed using **VS Code**

## Packages

```
$ sudo apt-get install libpcre2-dev pkg-config libsystemd-dev openssl llvm clang libc6-dev-i386 libc6 cmake zlib1g zlib1g-dev libssl-dev
```

## libssh

[website](https://www.libssh.org/)

```
$ git clone https://git.libssh.org/projects/libssh.git libssh
$ cd libssh
$ git checkout stable-0.9
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## libyang

[GitHub](https://github.com/CESNET/libyang)

```
$ git clone https://github.com/CESNET/libyang.git
$ cd libyang
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## sysrepo

[GitHub](https://github.com/sysrepo/sysrepo)

```
$ git clone https://github.com/sysrepo/sysrepo.git
$ cd sysrepo
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## libnetconf2

[GitHub](https://github.com/CESNET/libnetconf2)

```
$ git clone https://github.com/CESNET/libnetconf2.git
$ cd libnetconf2
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## netopeer2

[GitHub](https://github.com/CESNET/netopeer2)

```
$ git clone https://github.com/CESNET/netopeer2.git
$ cd netopeer2
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

In case of errors with last `make` command, run:

```
$ sudo ldconfig
$ sudo make install 
```

## Rust

[website](https://www.rust-lang.org/)

```
$ curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

(Note) Other instructions that have been executed for `keystone`:

```
$ rustup toolchain install nightly
$ rustup +nightly component add rust-src
$ rustup +nightly target add riscv64gc-unknown-none-elf
$ cargo +nightly install cargo-xbuild
```

## Docker

Follow the instructions at this [link](https://docs.docker.com/desktop/install/ubuntu/)

To start docker:

```
$ systemctl --user start docker-desktop
```

## Enarx

Follow the instructions at this [link](https://github.com/enarx/enarx/blob/main/docs/Install.md)

## wasi-sdk

[GitHub](https://github.com/WebAssembly/wasi-sdk)

```
$ git clone --recursive https://github.com/WebAssembly/wasi-sdk.git
$ cd wasi-sdk
$ git checkout wasi-sdk-17
$ git submodule update 
$ NINJA_FLAGS=-v make package
```

# Build TNED

## Clone repository

I have cloned it in `/home/giacomo/Documents/SPIRS/TNED/`

```
$ git clone --recursive https://gitlab.com/spirs_eu/TNED/ccips-cfgipsec.git
$ cd ccips-cfgipsec
```

## Fix the code

 1. copy files `parson.h` and `parson.c` from directory `parson/` to `src/`
 2. in file `CMakeLists.txt`, comment line 13 and uncomment line 12, updating that value 
 3. in file `cmd/i2nsf_enarx/enarx/build.rs`, update line 8 and 12

(Note) My paths:
 - `CMakeLists.txt`, line 12: `/home/giacomo/Documents/programs/wasi-sdk/build/wasi-sdk-17.0/bin/clang`
 - `build.rs`, line 8: `/home/giacomo/Documents/SPIRS/TNED/ccips-cfgipsec/libs`
 - `build.rs`, line 12: `/home/giacomo/Documents/SPIRS/TNED/ccips-cfgipsec/libs/trust_handler.h`

## Build

Execute these options starting from `ccips-cfgipsec` directory

### Option 1

```
$ mkdir build
$ cd build
$ cmake .. 
$ make
```

### Option 2 - RA

```
$ mkdir build_ra
$ cd build_ra
$ cmake -DENARX_RA=ON ..
$ make
```

### Option 3 - TA

Build libraries:

```
$ mkdir build_ta
$ cd build_ta
$ cmake -DENARX_TA=ON ..
$ make
$ make install
```

Build enarx application:

```
$ cd ../cmd/i2nsf_enarx/enarx
$ cargo build --release --target=wasm32-wasi
```

In case of errors with `cargo`, run:

```
$ rustup target add wasm32-wasi
$ cargo build --release --target=wasm32-wasi
```

Launch docker container:

```
$ docker run --rm --name enarx_ccips -v `pwd`/Enarx.toml:/enarx.toml -p 10000:10000  -v `pwd`/target/wasm32-wasi/release/enarx.wasm:/i2nsf_enarx.wasm enarx/enarx:0.6.3 enarx run --wasmcfgfile /enarx.toml /i2nsf_enarx.wasm
```
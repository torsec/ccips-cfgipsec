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

 1. in file `CMakeLists.txt`, comment line 13 and uncomment line 12, updating that value 
 2. in file `cmd/i2nsf_enarx/enarx/build.rs`, update line 8 and 12

(Note) My paths:
 - `CMakeLists.txt`, line 12: `/home/giacomo/Documents/programs/wasi-sdk/build/wasi-sdk-17.0/bin/clang`
 - `build.rs`, line 8: `/home/giacomo/Documents/SPIRS/TNED/ccips-cfgipsec/libs`
 - `build.rs`, line 12: `/home/giacomo/Documents/SPIRS/TNED/ccips-cfgipsec/libs/trust_handler.h`

## Setup sysrepo

```
$ sudo sysrepoctl -i yang/ietf-inet-types.yang  --permissions=666 -v3
$ sudo sysrepoctl -i yang/ietf-netconf-acm.yang  --permissions=666 -v3
$ sudo sysrepoctl -i yang/ietf-yang-types.yang  --permissions=666 -v3
$ sudo sysrepoctl -i yang/ietf-i2nsf-ikec.yang  --permissions=666 -v3
$ sudo sysrepoctl -i yang/ietf-i2nsf-ikeless.yang  --permissions=666 -v3
$ sudo sysrepoctl -c ietf-i2nsf-ikeless -e ikeless-notification  -v3
```


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
$ rustup target add wasm32-wasi
$ cargo build --release --target=wasm32-wasi
```

## Run the demo

Execute these options starting from `ccips-cfgipsec` directory

### Untrusted

```
$ cd build 
$ sudo ./i2nsf_cfgipsec2 -v 5
```

### Trusted

Spawn two terminals:

1. Launch docker container:

```
$ cd cmd/i2nsf_enarx/enarx
$ docker run --rm --name enarx_ccips -v `pwd`/Enarx.toml:/enarx.toml -p 10000:10000  -v `pwd`/target/wasm32-wasi/release/enarx.wasm:/i2nsf_enarx.wasm enarx/enarx:0.6.3 enarx run --wasmcfgfile /enarx.toml /i2nsf_enarx.wasm
```

2. Launch executable:

```
$ cd build_ra 
$ sudo ./i2nsf_cfgipsec2 -v 5
```

# Keystone

## Docs

Thesis:
* [Bruno](https://webthesis.biblio.polito.it/secure/29457/1/tesi.pdf) (fix: ```$ sudo apt install python3-pip```)
* [Ciravegna](https://webthesis.biblio.polito.it/secure/28633/1/tesi.pdf)
* [Donnini](https://webthesis.biblio.polito.it/secure/27657/1/tesi.pdf)

Current repository: [keystone](https://gitlab.com/valerio_donnini/keystone)

Fixes:

```
$ sudo apt install quilt
```

## Build Keystone applications

1. After having built **`keystone`**, execute these commands starting from `ccips-cfgipsec` directory:

```
$ mkdir build_k
$ cd build_k
$ cmake -DKEYSTONE=ON ..
$ make
$ make keystone-ccips-package
```

2. Copy the file `build_k/keystone-ccips.ke` into `<keystone_build_dir>/overlay/root/`

3. Execute this command starting from `<keystone_build_dir>` directory:

```
$ make image
```

## Run Keystone applications

Execute these commands starting from `<keystone_build_dir>` directory:

```
$ ./scripts/run-qemu.sh
# ./keystone-ccips.ke
```

extern crate cmake;
use std::env;
use std::path::PathBuf;
fn main()
{
    // let mut config = cmake::Config::new("../../../.");
    // config.define("WASI_TARGET", "ON");
    // config.define("USE_SYSREPO", "OFF");
    // config.define("CMAKE_INSTALL_PREFIX", "/home/debian/ipsec_tests/ipsec_interactor/build/");
    

    // let dst = config.build();

    // let dst = cmake::build("../../../.");
    // println!("cargo:rustc-link-search=native=/usr/local/lib");
    // println!("cargo:rustc-link-lib=static=i2nsf");
    // println!("cargo:rustc-link-lib=static=parson");
    

    // println!("cargo:rustc-link-lib=i2nsf");
    // println!("cargo:rustc-link-lib=parson");
    // println!("cargo:rustc-target=wasm32-wasi");
    // println!("cargo:rerun-if-changed=your_lib_bg.js");  
    // let out_dir = std::env::var("OUT_DIR").unwrap();
    // let _ = Command::new("wasm-bindgen").args(&[
    //     "your_lib.so",
    //     "--out-dir",
    //     &out_dir,
    //     "--no-modules",
    //     "--no-typescript",
    // ])
    // .status();

        // let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
        // println!("cargo:rustc-link-search={}", out_dir.display());
        // println!("cargo:rustc-link-search=native=/usr/local/lib");
        // println!("cargo:rustc-link-lib=static=i2nsf");
        // println!("cargo:rustc-link-lib=static=parson");


    // let target = env::var("TARGET").unwrap();
    // if target == "wasm32-wasi" {
    //     let mut lib_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    //     lib_dir.push("/usr/local/lib");
    //     println!("cargo:rustc-link-search=native={}", lib_dir.display());
    //     println!("cargo:rustc-link-lib=i2nsf");
    //     println!("cargo:rustc-link-lib=parson");
    // }

    println!("cargo:rustc-link-search=native=/home/debian/ipsec_tests/ipsec_interactor/libs");
    println!("cargo:rustc-link-lib=static=i2nsf");
    let llvm_config_path = "/usr/bin/llvm-config";
    env::set_var("LLVM_CONFIG_PATH", llvm_config_path);
    // println!("cargo:rustc-link-search=native=/home/debian/ipsec_tests/ipsec_interactor/libs");
    // println!("cargo:rustc-link-lib=static=i2nsf");

    let bindings = bindgen::Builder::default()
        .header("/home/debian/ipsec_tests/ipsec_interactor/libs/trust_handler.h")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");


}
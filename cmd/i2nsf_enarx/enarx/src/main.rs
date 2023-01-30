
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
fn main() {
    let data = "test".as_bytes();
    let result = unsafe { handle_message(data.as_ptr() as *mut _) };
    println!("Result: {:?}", result);
    // Use the C functions in your Rust code
    // let data = "Hello, World!".as_ptr() as *const libc::c_char;
    // let result = unsafe { handle_message(data) };
    // println!("{}", unsafe { CStr::from_ptr(result) }.to_str().unwrap());
}

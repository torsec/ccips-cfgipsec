
// include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ffi::{CStr, CString};


extern "C" {
    fn handle_message(data: *const libc::c_char) -> *mut libc::c_char;
}

pub fn handle_message_rust(data: &str) -> String {
    let data_c_str = CString::new(data).unwrap();
    let data_ptr = data_c_str.as_ptr();

    unsafe {
        let result = handle_message(data_ptr);
        let result_str = CStr::from_ptr(result).to_str().unwrap();
        libc::free(result as *mut libc::c_void);
        String::from(result_str)
    }
}


fn main() {
   let result = handle_message_rust("test");
   println!("Result: {:?}", result);
}
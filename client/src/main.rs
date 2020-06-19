use std::io::prelude::*;
use std::net::TcpStream;

fn main() {
    let ip = "127.0.0.1";
    let port = "5678";
    let mut stream = TcpStream::connect(format!("{}:{}", ip, port));

    println!("Hello, world!");
}

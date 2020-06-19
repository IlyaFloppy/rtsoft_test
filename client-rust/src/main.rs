use std::io::prelude::*;
use std::net::{TcpStream, Shutdown};
use std::{fs, env};

fn transform_u32_to_array_of_u8(x: u32) -> [u8; 4] {
    let b1: u8 = ((x >> 24) & 0xff) as u8;
    let b2: u8 = ((x >> 16) & 0xff) as u8;
    let b3: u8 = ((x >> 8) & 0xff) as u8;
    let b4: u8 = (x & 0xff) as u8;
    return [b1, b2, b3, b4];
}

fn send_data_block(stream: &mut TcpStream, filename: &str, data: &Vec<u8>, first_block: bool) -> std::io::Result<()> {
    if first_block {
        stream.write(filename.as_bytes())?;
        stream.write(&[0 as u8])?;
    }
    stream.write(transform_u32_to_array_of_u8(data.len() as u32).as_ref())?;
    stream.write(data)?;

    Ok(())
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() != 4 {
        println!("usage:\nclient-rust <ip> <local file> <remote file>");
        return Ok(());
    }

    let ip = args.get(1).unwrap();
    let port = "5678";
    let local_filename = args.get(2).unwrap();
    let remote_filename = args.get(3).unwrap();

    let mut stream = TcpStream::connect(format!("{}:{}", ip, port))
        .expect("failed to connect to server");

    let data = fs::read(local_filename)
        .expect("unable to read file");

    let mut buffer: Vec<u8> = Vec::new();
    let mut first_block = true;
    for byte in data {
        if buffer.len() < 3 {
            buffer.push(byte);
        } else {
            send_data_block(&mut stream, remote_filename, &buffer, first_block)?;
            first_block = false;
            buffer.clear();
            buffer.push(byte);
        }
    }
    if buffer.len() > 0 {
        send_data_block(&mut stream, remote_filename, &buffer, first_block)?;
        first_block = false;
        buffer.clear();
    }
    send_data_block(&mut stream, remote_filename, &Vec::new(), first_block)?;


    stream.shutdown(Shutdown::Both)
        .expect("shutdown call failed");

    Ok(())
}

import socket

from sys import argv


def send_data_block(buffer, file_name, first_block):
    if first_block:
        sock.send(file_name.encode('ascii', 'replace'))
        sock.send(b'\0')
    length = (len(buffer)).to_bytes(4, 'big')
    sock.send(length)
    sock.send(b''.join(buffer))


if len(argv) != 4:
    print("Insufficient data entered.\n"
          "Check for the following parameters:\n"
          "script name\n"
          "address\n"
          "local name\n"
          "remote name\n")
script, address, local_name, remote_name = argv

sock = socket.socket()
sock.connect((address, 5678))

with open(local_name, "rb") as local_file:
    buffer = []
    first_block = True
    byte = local_file.read(1)
    while byte:
        buffer.append(byte)
        if len(buffer) == 1024:
            send_data_block(buffer, remote_name, first_block)
            first_block = False
            buffer = []
        byte = local_file.read(1)
    if len(buffer) > 0:
        send_data_block(buffer, remote_name, first_block)
        first_block = False
    send_data_block([], remote_name, False)
sock.close()

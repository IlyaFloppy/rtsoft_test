import socket

from sys import argv


def send_data_block(buffer, file_name, first_block):
    length = (len(buffer)).to_bytes(4, 'big')
    sock.send(length)
    if first_block:
        sock.send(file_name.encode("utf-8"))
        sock.send(b'\0')
    sock.send(buffer)


script, address, local_name, remote_name = argv
sock = socket.socket()
sock.connect((address, 5678))
print("адрес:", address, "loc_name:", local_name, "rem_name:", remote_name)
with open(local_name, "rb") as local_file:
    buffer = []
    first_block = True
    while byte := local_file.read(1):
        buffer.append(byte)
        if len(buffer) == 1024:
            send_data_block(buffer, remote_name, first_block)
            first_block = False
            buffer = []
    if len(buffer) > 0:
        send_data_block(buffer, remote_name, first_block)
        first_block = False
    send_data_block([], remote_name, False)
sock.close()

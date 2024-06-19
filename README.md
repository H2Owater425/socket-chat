# Socket Chat
Simple chat made with Winsock2

## Building
```sh
gcc main.c -o chat -lws2_32
```

## Usage
```
Usage: chat [ip] <port>

Options:
  <port>             port number
  ip                 ip address, use to behave as client
```
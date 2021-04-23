# pacer
Test code for Pacer protocol

# Ports

This test code uses UDP port 8463

# sendtest

This sends a packet once per second to the given IP address.   It can broadcast.

Compiling:

`gcc -o sendtest sendtest.c -Ofast`

Running:


    ./sendtest 127.0.0.1
    ./sendtest 255.255.255.255
    ./sendtest 10.1.0.2


# recvtest 

This recieves packets and prints some diagnostics

Compiling:

`gcc -o recvtest recvtest.c -Ofast`

Running:

`./recvtest`

Example output:

                ====================== Received 76 bytes
                00 00 02 00 00 00 00 00  00 00 00 00 00 00 00 00  |  ................ 
                00 00 00 00 D2 04 00 00  0D 00 00 00 E2 EB 82 60  |  ...............` 
                00 00 00 00 F4 43 0C 00  40 42 0F 00 00 00 00 00  |  .....C..@B...... 
                00 00 00 00 00 00 00 00  00 00 00 00 0D 00 00 00  |  ................ 
                00 00 00 00 00 00 00 00  E8 03 00 00              |  ............ 
                Version: 0  Message Type: 0  Timecode hint: 0
                Sender ID: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
                Timeline ID: 1234  Sequence: 13
                Timeline Status: Running (2)
                REC 1619192802:804952/1000000
                REF 1619192802:803828/1000000 (REC-REF: 0:1124)
                OFF 0:0/0
                TIM 13:0/1000


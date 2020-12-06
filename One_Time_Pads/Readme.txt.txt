Author: Nag Balijepalli
Date: 11/30/2020
Name: One Time Pads (Client & Server -> Encryption)

Description: Program will encrypt and decrypt plaintext into ciphertext, using a key, in exactly the same fashion as above, except it will be using modulo 27 operations: your 27 characters are the 26 capital letters, and the space character. The program uses Network IPC API's such as socket, connect, bind, listen & accept.


To compile: compileall
To run test script: ./p5testscript <RANDOM PORT> <RANDOM PORT> <redirection or stdout>

The TestScript will run encryption, decryption, concurrency between client and server.
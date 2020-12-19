- Author: Nag Balijepalli
- Name: Multithreaded processing
- Date: 11/16/2020
-------
-To run: gcc --std=gnu99 -lpthread -g -o program main.c.

Note:
This program will take in a file input and start processing immediatly following a producer-consumer process until it hits a STOP. It will change every ++ to a ^ and only print 80 charectors per line.
- Thread 1: take user input.
  -- buffer #1
- Thread 2: remove '/n' to make processing easier
  -- buffer #2
- Thread 3: remove "++" to a "^"
  -- buffer #3
- Thread 4: ouptut 80 charectors per line.

![Screenshot](buffer_threads.PNG)

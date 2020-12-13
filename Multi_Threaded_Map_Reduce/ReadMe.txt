Author: Nag Balijepalli
Date: 12/12/2020
Name: Multi Threaded Map Reduce Using Rust.

MapReduce is a programming model and an associated implementation for processing and generating big data sets with a parallel, distributed algorithm on a cluster.
A MapReduce program is composed of a map procedure, which performs filtering and sorting, and a reduce method, 
which performs a summary operation (such as counting the number of students in each queue, yielding name frequencies).

In this program you will see 2 different map reductions being done. 
#1 using 2 threads - this will act as your expected output
#2 using X amount of threads depending on the num_of_partitions. 

To compile: rustc main.rs
to run: ./main <num_of_partitions> <num_elements>

Examples:
1) ./main 5 150
2) ./main 10 3478

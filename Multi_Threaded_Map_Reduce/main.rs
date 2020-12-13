/*
Author: Nag Balijepalli
Name: Multi-Threaded Map Reduce using Rust
Date: Dec 5th 2020
*/
use std::env;
use std::thread;

fn print_partition_info(vs: &Vec<Vec<usize>>){
    println!("Number of partitions = {}", vs.len());
    for i in 0..vs.len(){
        println!("\tsize of partition {} = {}", i, vs[i].len());
    }
}

fn generate_data(num_elements: usize) -> Vec<usize>{
    let mut v : Vec<usize> = Vec::new();
    for i in 0..num_elements {
        v.push(i);
    }
    return v;
}

/*
* Partition the data in the vector v into 2 vectors
*/
fn partition_data_in_two(v: &Vec<usize>) -> Vec<Vec<usize>>{
    let partition_size = v.len() / 2;
    // Create a vector that will contain vectors of integers
    let mut xs: Vec<Vec<usize>> = Vec::new();
    let mut x1 : Vec<usize> = Vec::new();     // Create the first vector of integers
    for i in 0..partition_size{
        x1.push(v[i]);
    }
    xs.push(x1);

    let mut x2 : Vec<usize> = Vec::new();     // Create the second vector of integers (2nd half)
    for i in partition_size..v.len(){
        x2.push(v[i]);
    }
    xs.push(x2);
    xs
}

/*
* Sum up the all the integers in the given vector
*/
fn map_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* Sum up the all the integers in the given vector
*/
fn reduce_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* A Multi threaded map-reduce program
*/
fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("ERROR: Usage {} num_partitions num_elements", args[0]);
        return;
    }
    let num_partitions : usize = args[1].parse().unwrap();
    let num_elements : usize = args[2].parse().unwrap();
    if num_partitions < 1{
      println!("ERROR: num_partitions must be at least 1");
        return;
    }
    if num_elements < num_partitions{
        println!("ERROR: num_elements cannot be smaller than num_partitions");
        return;
    }

    // Generate data.
    let v = generate_data(num_elements);

    // PARTITION STEP: partition the data into 2 partitions
    let xs = partition_data_in_two(&v);
    print_partition_info(&xs);
    let mut intermediate_sums : Vec<usize> = Vec::new();
    // 2 threads that run concurrently and each of which uses map_data() function to process one of the two partitions
    let mut array_uno = vec![];

    for i in 0..2{
        let copy_xs = xs[i].clone();
        array_uno.push(thread::spawn(move||map_data(&copy_xs)));
    }
    for kids in array_uno {
        intermediate_sums.push(kids.join().unwrap()); 
    }
    
    println!("Intermediate sums = {:?}", intermediate_sums);

    // REDUCE STEP: Process the intermediate result to produce the final result
    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);
    let mut intermediate_sums_c : Vec<usize> = Vec::new();
    let xss = partition_data(num_partitions,&v);
    print_partition_info(&xss);
    
    let mut kiddo_thread_array = vec![];
    for i in 0..num_partitions{ // a thread for each partition that will be processed in map_data()
        let copy_xss = xss[i].clone();
            kiddo_thread_array.push(thread::spawn(move || map_data(&copy_xss)
        ));
    }
    
    for kids in kiddo_thread_array {
        intermediate_sums_c.push(kids.join().unwrap()); 
    }
    println!("Intermediate sums = {:?}", intermediate_sums_c);
    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);
}

/*
* Partitions the data into a number of partitions such that
*/
fn partition_data(num_partitions: usize, v: &Vec<usize>) -> Vec<Vec<usize>>{
    let partition_size = v.len() / num_partitions;
    let mut remainder = v.len() % num_partitions;
    let mut initial_size = 0;
    let mut xss: Vec<Vec<usize>> = Vec::new(); //vector that will contain vectors of integers
    // if remainder > 0 {
        for _x in 0..num_partitions{
            // Create the first vector of integers
            let mut x1 : Vec<usize> = Vec::new();   
            if remainder > 0 {
                for i in initial_size..partition_size+1+initial_size{
                    x1.push(v[i]);
                }
                initial_size += partition_size+1;
                remainder = remainder -1;
                xss.push(x1);
            } else {
                for i in initial_size..partition_size + initial_size{
                    x1.push(v[i]);
                }
                xss.push(x1);
                initial_size += partition_size;
            }
        }
    xss
}

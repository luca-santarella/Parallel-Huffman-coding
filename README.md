# Huffman Coding (SPM project)
## Problem
The aim of the project is to implement the Huffman coding in parallel using native threads and FastFlow.
The Huffman coding is a kind of encoding algorithm with variable-length codes that can be used to compress data such as text file, in our case the input is an ASCII file.

## Programming Language/Framework
C++ only for the native threads implementation, C++ and FastFlow (https://github.com/fastflow/fastflow) for the second version using the FastFlow framework.

## Methodologies
After analyzing the problem by understanding the weights and providing a sequential version of the code, the idea was to use the data parallel pattern <i>map</i> to deal with these heavy computing steps of the algorithm.
The solution adopted is to divide the string into substrings which are allocated to workers, each worker elaborates the substring and computes the subresult into a shared data structure which is made thread-safe for access.
Finally the recombination of the subresults yield the final result, this technique is adopted for each of the parallelizable phases of the problem.

## Results
For the test text files that we used the compressed file is around 50% smaller than the original file. 
The best speedup achieved for the native threads version is around 4x with 32 threads and the best speedup achieved with the FastFlow implementation is 3.8x with 32 threads.
The table below shows the execution times (in seconds) for the 100 MB text file tested.

![image](https://github.com/luca-santarella/SPM-proj/assets/67547343/aa71bca1-17ea-48d1-8d18-7ffdc9a453c0)


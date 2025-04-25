# Bacteria Simulation (Game of Life) - Serial & Parallel Implementations in C with OpenMP

This project simulates the evolution of a 2D bacteria colony (Conway's Game of Life) over multiple generations. It includes both a **serial** implementation and four **parallel** versions using **OpenMP**, to compare performance and correctness.

## Features

Serial implementation of the Game of Life
Four parallel versions using OpenMP:
  - **V1**: Uses parallel for
  - **V2**: Uses parallel for but lets threads be created once in the beginning
  - **V3**: Uses manual specific data partitioning
  - **V4**: Uses OpenMP tasks
Performance benchmarking and speedup analysis
Output verification: compares parallel results to the serial version

## Input Format
-  The input file must have the following format:
<ROWS> <COLS>
....X..
...XX..
..X....
.......
-  Each cell is either X (alive) or . (dead).
-  First line defines the grid size.

## Output Files
-  For an input file bacteria.txt, the following output files are generated:
bacteria.txt_serial_out.txt
bacteria.txt_parallel_omp_V1.txt
bacteria.txt_parallel_omp_V2.txt
bacteria.txt_parallel_omp_V3.txt
bacteria.txt_parallel_omp_V4.txt
Each contains the final grid after all generations

## Debug Mode
-  To enable debug printing of the grid after each generation, uncomment the #define DEBUG line at the top of the file.

## Performance Results
-  Detailed benchmarking results, including times for each test and configuration, can be found in the [bacteria results.xlsx](bacteria results.xlsx).

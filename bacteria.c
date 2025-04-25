#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

//#define DEBUG //se sterge comentariul pentru a folosi modul DEBUG

#define MAX_ROWS 10000
#define MAX_COLS 10000

int ROWS, COLS;
int generations;
int *grid, *new_grid;
int *parallel_grid;

void swap_grids(int **grid1, int **grid2);
void print_grid(int *grid);
void read_input_file(const char *filename);
void write_output_file(const char *filename, int *grid_to_save);
int count_neighbors(int *grid, int x, int y);
void simulate_serial(int generations);
void parallel_v1(int generations, int num_threads);
int compare_grids(int *grid1, int *grid2);
void parallel_v2(int generations, int num_threads);
void parallel_v3(int generations, int num_threads);
void parallel_v4(int generations, int num_threads);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input_file> <generations> <num_threads>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    generations = atoi(argv[2]);
    int num_threads = atoi(argv[3]);

    char serial_output_file[256], parallel_output_file1[256], parallel_output_file2[256], parallel_output_file3[256], parallel_output_file4[256];
    snprintf(serial_output_file, sizeof(serial_output_file), "%s_serial_out.txt", input_file);
    snprintf(parallel_output_file1, sizeof(parallel_output_file1), "%s_parallel_omp_V1.txt", input_file);
    snprintf(parallel_output_file2, sizeof(parallel_output_file2), "%s_parallel_omp_V2.txt", input_file);
    snprintf(parallel_output_file3, sizeof(parallel_output_file3), "%s_parallel_omp_V3.txt", input_file);
    snprintf(parallel_output_file4, sizeof(parallel_output_file4), "%s_parallel_omp_V4.txt", input_file);

    read_input_file(input_file);

    int *serial_grid = (int *)malloc(ROWS * COLS * sizeof(int));
    if (!serial_grid) {
        printf("Memory allocation error for serial_grid\n");
        return 1;
    }

    double start_time, end_time;

    //Simularea seriala
    printf("Running serial simulation...\n");
    start_time = omp_get_wtime();
    simulate_serial(generations);
    end_time = omp_get_wtime();
    memcpy(serial_grid, grid, ROWS * COLS * sizeof(int));
    write_output_file(serial_output_file, grid);
    printf("Serial output written to %s\n", serial_output_file);
    double serial_time = end_time - start_time;

    read_input_file(input_file);

    //Simularea paralela V1
    printf("Running parallel v1 simulation...\n");
    start_time = omp_get_wtime();
    parallel_v1(generations, num_threads);
    end_time = omp_get_wtime();
    double parallel_v1_time = end_time - start_time;
    write_output_file(parallel_output_file1, grid);

    int identical = compare_grids(serial_grid, grid);
    printf("Results serial and parallel v1 are %s\n", identical ? "identical" : "different");

    //Simularea paralela V2
    printf("Running parallel v2 simulation...\n");
    read_input_file(input_file);
    start_time = omp_get_wtime();
    parallel_v2(generations, num_threads);
    end_time = omp_get_wtime();
    double parallel_v2_time = end_time - start_time;
    write_output_file(parallel_output_file2, grid);

    identical = compare_grids(serial_grid, grid);
    printf("Results serial and parallel v2 are %s\n", identical ? "identical" : "different");

    //Simularea paralela V3
    printf("Running parallel v3 simulation...\n");
    read_input_file(input_file);
    start_time = omp_get_wtime();
    parallel_v3(generations, num_threads);
    end_time = omp_get_wtime();
    double parallel_v3_time = end_time - start_time;
    write_output_file(parallel_output_file3, grid);

    identical = compare_grids(serial_grid, grid);
    printf("Results serial and parallel v3 are %s\n", identical ? "identical" : "different");

    //Simularea paralela V4
    printf("Running parallel v4 simulation...\n");
    read_input_file(input_file);
    start_time = omp_get_wtime();
    parallel_v4(generations, num_threads);
    end_time = omp_get_wtime();
    double parallel_v4_time = end_time - start_time;
    write_output_file(parallel_output_file4, grid);

    identical = compare_grids(serial_grid, grid);
    printf("Results serial and parallel v4 are %s\n", identical ? "identical" : "different");

    printf("\n--- Performance Summary ---\n");
    printf("Serial simulation time: %.6f seconds\n", serial_time);

    printf("Parallel v1 simulation time: %.6f seconds (Speedup: %.4f)\n", parallel_v1_time, serial_time / parallel_v1_time);
    printf("Parallel v2 simulation time: %.6f seconds (Speedup: %.4f)\n", parallel_v2_time, serial_time / parallel_v2_time);
    printf("Parallel v3 simulation time: %.6f seconds (Speedup: %.4f)\n", parallel_v3_time, serial_time / parallel_v3_time);
    printf("Parallel v4 simulation time: %.6f seconds (Speedup: %.4f)\n", parallel_v4_time, serial_time / parallel_v4_time);


    free(serial_grid);
    free(grid);
    free(new_grid);
    free(parallel_grid);

    return 0;
}

void swap_grids(int **grid1, int **grid2) {
    int *temp = *grid1;
    *grid1 = *grid2;
    *grid2 = temp;
}


void print_grid(int *grid) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i * COLS + j] == 1) {
                printf("X");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

void read_input_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    fscanf(file, "%d %d", &ROWS, &COLS);
    grid = (int *)malloc(ROWS * COLS * sizeof(int));
    new_grid = (int *)malloc(ROWS * COLS * sizeof(int));
    parallel_grid = (int *)malloc(ROWS * COLS * sizeof(int));

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            char ch;
            fscanf(file, " %c", &ch);
            grid[i * COLS + j] = (ch == 'X') ? 1 : 0;
        }
    }

    fclose(file);
}

void write_output_file(const char *filename, int *grid_to_save) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            fprintf(file, "%c", grid_to_save[i * COLS + j] ? 'X' : '.');
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int count_neighbors(int *grid, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + ROWS) % ROWS;
            int ny = (y + dy + COLS) % COLS;
            count += grid[nx * COLS + ny];
        }
    }
    return count;
}

void simulate_serial(int generations) {
    for (int gen = 0; gen < generations; gen++) {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int neighbors = count_neighbors(grid, i, j);
                if (grid[i * COLS + j] == 1 && (neighbors == 2 || neighbors == 3)) {
                    new_grid[i * COLS + j] = 1;
                } else if (grid[i * COLS + j] == 0 && neighbors == 3) {
                    new_grid[i * COLS + j] = 1;
                } else {
                    new_grid[i * COLS + j] = 0;
                }
            }
        }
        swap_grids(&grid, &new_grid);
    }
}

int compare_grids(int *grid1, int *grid2) {
    for (int i = 0; i < ROWS * COLS; i++) {
        if (grid1[i] != grid2[i]) {
            return 0;
        }
    }
    return 1;
}

void parallel_v1(int generations, int num_threads) {
    for (int gen = 0; gen < generations; gen++) {
#pragma omp parallel for num_threads(num_threads) collapse(2) schedule(static)
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int neighbors = count_neighbors(grid, i, j);
                parallel_grid[i * COLS + j] = (grid[i * COLS + j] == 1)? (neighbors == 2 || neighbors == 3): (neighbors == 3);
            }
        }
        swap_grids(&grid, &parallel_grid);
        #ifdef DEBUG
            printf("After generation %d:\n", gen);
            print_grid(grid);
        #endif
    }
}

void parallel_v2(int generations, int num_threads) {
    #pragma omp parallel num_threads(num_threads) default(none) shared(grid, parallel_grid, ROWS, COLS, generations) 
    {
        int gen, i, j, neighbors;

        for (gen = 0; gen < generations; gen++) {
            #pragma omp for collapse(2) schedule(static)
            for (i = 0; i < ROWS; i++) {
                for (j = 0; j < COLS; j++) {
                    neighbors = count_neighbors(grid, i, j);
                    parallel_grid[i * COLS + j] = (grid[i * COLS + j] == 1)
                                                  ? (neighbors == 2 || neighbors == 3)
                                                  : (neighbors == 3);
                }
            }

            #pragma omp barrier

            #pragma omp single
            {
                swap_grids(&grid, &parallel_grid);
                #ifdef DEBUG
                    printf("After generation %d:\n", gen);
                    print_grid(grid);
                #endif
            }

            #pragma omp barrier
        }
    }
}

void parallel_v3(int generations, int num_threads) {
    int block_size = ROWS / num_threads; 

    #pragma omp parallel num_threads(num_threads) default(none) shared(grid, parallel_grid, ROWS, COLS, generations, block_size, num_threads)
    {
        int thread_id = omp_get_thread_num();
        int start_row = thread_id * block_size;             
        int end_row = (thread_id == num_threads - 1) ? ROWS : (thread_id + 1) * block_size; 

        for (int gen = 0; gen < generations; gen++) {
            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < COLS; j++) {
                    int neighbors = count_neighbors(grid, i, j);
                    parallel_grid[i * COLS + j] = (grid[i * COLS + j] == 1)
                                                  ? (neighbors == 2 || neighbors == 3)
                                                  : (neighbors == 3);
                }
            }
            #pragma omp barrier
            #pragma omp single
            {
                swap_grids(&grid, &parallel_grid);
                #ifdef DEBUG
                    printf("After generation %d:\n", gen);
                    print_grid(grid);
                #endif
            }
            #pragma omp barrier
        }
    }
}

void parallel_v4(int generations, int num_threads) {
    #pragma omp parallel num_threads(num_threads) default(none) shared(grid, parallel_grid, ROWS, COLS, generations)
    {
        #pragma omp single
        {
            for (int gen = 0; gen < generations; gen++) {
                for (int i = 0; i < ROWS; i++) {
                    #pragma omp task firstprivate(i) shared(grid, parallel_grid, COLS)
                    {
                        for (int j = 0; j < COLS; j++) {
                            int neighbors = count_neighbors(grid, i, j);
                            parallel_grid[i * COLS + j] = (grid[i * COLS + j] == 1)
                                                          ? (neighbors == 2 || neighbors == 3)
                                                          : (neighbors == 3);
                        }
                    }
                }

                #pragma omp taskwait

                swap_grids(&grid, &parallel_grid);
                #ifdef DEBUG
                    printf("After generation %d:\n", gen);
                    print_grid(grid);
                #endif
            }
        }
    }
}


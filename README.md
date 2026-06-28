# K-Means Clustering Implementation

An efficient implementation of the **K-means clustering algorithm** written from scratch in both **C** and **Python**. This project showcases data structure manipulation, dynamic memory allocation, and algorithmic optimization.

## Features
* **Dual Implementation:** Includes both a low-level memory-optimized version in C and a high-level scripting version in Python.
* **Dynamic Handling:** Custom file parsing mechanism that dynamically scales memory to read datasets of any dimensionality.
* **Input Validation:** Robust input verification logic ensuring proper parameter ranges ($k$ clusters and max iterations).

## Technologies Used
* **C** (Dynamic Memory Allocation, File I/O, Pointers, Math)
* **Python**

## How to Run (C Version)
Compile the C program:
```bash
gcc -ansi -Wall -Wextra -pedantic -O3 kmeans.c -lm -o kmeans
./kmeans <K_clusters> <max_iter> < input_data.txt

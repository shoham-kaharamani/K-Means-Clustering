#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* builds the full flat-array out of the input file*/
/* we must call the pointers since we need to edit n and k*/
double* text_to_array(int *d_ptr, int *N_ptr) {
    int capacity = 10;
    int size = 0;
    double *data = malloc(capacity * sizeof(double));
    
    int current_d = 0;
    double val;
    char ch;
/* scans each number (float) in the input file*/
    while (scanf("%lf", &val) == 1) {
        if (size >= capacity) {
            capacity *= 2;
            data = realloc(data, capacity * sizeof(double));
        }
        data[size++] = val;

        current_d++;

/* checks if we have reached the end of a line, in order to extract the dimension */
        ch = getchar();
        
        if (ch == '\n') {
            if (*d_ptr == 0) {
                *d_ptr = current_d; 
            }
            current_d = 0;
        }
    }
    *N_ptr = size / *d_ptr;
    
    return data; 
}
/* makes sure that given string is combined from digits only*/
int is_positive_integer(char *str) {
    int i;
    if (str[0] == '\0') return 0;
    
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

int K_validation(int argc, char* *argv, int N) {
    int k;

    /* checks if the number of input arguments is reasonable*/
    if (argc != 2 && argc != 3) {
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /* validates that the k argument is a positive integer */
    if (!is_positive_integer(argv[1])) {
        printf("Incorrect number of clusters!\n");
        exit(1);
    }

    /* converts the array of digit-ascii values to a number */
    k = atoi(argv[1]);


    /* validates k is in the proper range */
    if (k <= 1 || k >= N) {
        printf("Incorrect number of clusters!\n");
        exit(1);
    }

    return k;
}


int iter_validation(int argc, char* *argv) {
    int iter = 0;
    /* takes care the default case when none iter value was provided */
    if (argc == 2) {
        iter = 400;
        return iter;
    }

    /* makes sure iter number is a positive integer*/
    if (!is_positive_integer(argv[2])) {
        printf("Incorrect maximum iteration!\n");
        exit(1);
    }

    /* converts the array of digit-ascii values to a number */
    iter = atoi(argv[2]);


    /* makes sure iter is in the proper range */
    if (iter <= 1 || iter >= 800) {
        printf("Incorrect maximum iteration!\n");
        exit(1);
    }

    return iter;
}

/* chooses the first k points as initial centroids */
double* initialize_k_centroids(double *data, int k, int d) {
    int i = 0;
    double *centroids_list = malloc(k * d * sizeof(double));
    for (i = 0; i < k * d; i++) {
        centroids_list[i] = data[i];
    }
    return centroids_list;
}

/* calculates the Euclidean distance between two points (2 pointers of d-array beginnigs)*/
double distance(double *point1, double *point2, int d) {
    int i = 0;
    double sum = 0.0;
    for (i = 0; i < d; i++) {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/* using the distance function, we call the centroids list, and find the closest one out of k*/
int find_closest_centroid(double *point, double *centroids_list, int k, int d) {
    int i = 0;
    int closest_centroid_index = 0;
    double closest_distance = distance(point, &centroids_list[0], d);
    
    for (i = 0; i < k; i++) {
        double current_distance = distance(point, &centroids_list[i * d], d);
        if (current_distance < closest_distance) {
            closest_distance = current_distance;
            closest_centroid_index = i;
        }
    }
    return closest_centroid_index;
}

/* initializes a new empty list for centroids */
double* initialize_new_empty_centroids_list(int k, int d) {
    int i = 0;
    double *new_centroids_list = malloc(k * d * sizeof(double));
    for (i = 0; i < k * d; i++) {
        new_centroids_list[i] = 0.0;
    }
    return new_centroids_list;
}

/* initializes the helper data structure for counting points assigned to each centroid */
int* initialize_new_centroids_list_counter(int k) {
    int i = 0;
    int *new_centroids_counter = malloc(k * sizeof(int));
    for (i = 0; i < k; i++) {
        new_centroids_counter[i] = 0;
    }
    return new_centroids_counter;
}

/* updates in-place the new centroids list, including the helper-counter*/
void update_new_centroids_list(double *data, double *new_centroids_list, int *new_centroids_counter, double *centroids_list, int k, int d, int N) {
    int i, j;
    for (i = 0; i < N; i++) {
        int closest_centroid_index = find_closest_centroid(&data[i * d], centroids_list, k, d);
        new_centroids_counter[closest_centroid_index]++;
        for (j = 0; j < d; j++) {
            new_centroids_list[closest_centroid_index * d + j] += data[i * d + j];
        }
    }
    return;
}

/* updates in-place the new centroids list, using the helper-counter as denominator*/
void update_centroids_list_avg(double *new_centroids_list, int *new_centroids_counter, int k, int d) {
    int i, j;
    for (i = 0; i < k; i++) {
        if (new_centroids_counter[i] > 0) {
            for (j = 0; j < d; j++) {
                new_centroids_list[i * d + j] /= new_centroids_counter[i];
            }
        }
    }
    return;
}

/* performs epsilon check - the stop condition*/
int epsilon_check(double *new_centroids_list, double *centroids_list, int k, int d, float epsilon) {
    int i;
    for (i = 0; i < k; i++) {
        if (distance(&new_centroids_list[i * d], &centroids_list[i * d], d) >= epsilon) {
            return 0;
        }
    }
    return 1;
}

/*after new centroids are calculated, update all arrays */
/*new centroids list in-place of centroids list, new list is being reset, counter-list as well*/
void update_all_arrays(double *centroids_list, double *new_centroids_list, int *new_centroids_counter, int k, int d) {
    int i;
    for (i = 0; i < k * d; i++) {
        centroids_list[i] = new_centroids_list[i];
        new_centroids_list[i] = 0.0;
    }
    for (i = 0; i < k; i++) {
        new_centroids_counter[i] = 0;
    }
    return;
}

/*main kmeans function, uses all other functions, and free memory in the end*/
double* kmeans(double *data, int k, int d, int N, int iter, float epsilon) {
    int i;
    double *centroids_list = initialize_k_centroids(data, k, d);
    double *new_centroids_list = initialize_new_empty_centroids_list(k, d);
    int *new_centroids_counter = initialize_new_centroids_list_counter(k);

    for (i = 0; i < iter; i++) {
        update_new_centroids_list(data, new_centroids_list, new_centroids_counter, centroids_list, k, d, N);
        update_centroids_list_avg(new_centroids_list, new_centroids_counter, k, d);

        if (epsilon_check(new_centroids_list, centroids_list, k, d, epsilon) == 1) {
                    update_all_arrays(centroids_list, new_centroids_list, new_centroids_counter, k, d);
            break;
        }

        update_all_arrays(centroids_list, new_centroids_list, new_centroids_counter, k, d);
    }

    free(new_centroids_list);
    free(new_centroids_counter);
    return centroids_list;
}




int main(int argc, char *argv[]) {
/*declare all variables*/
    int d = 0; 
    int N = 0; 
    int k;
    int iter;
    double *data;
    double* final_centroids_list;
    float epsilon = 0.001;
    int i, j;

/*creating the data array out of input file*/
/*updating d,N according to read file*/
    data = text_to_array(&d, &N);
/*validating k and iter inputs*/
    k = K_validation(argc, argv, N);
    iter = iter_validation(argc, argv);
/*executing main operation*/
    final_centroids_list = kmeans(data, k, d, N, iter, epsilon);
/*printing results according to required form*/
    for (i = 0; i < k; i++) {
        for (j = 0; j < d; j++) {
            printf("%.4f", final_centroids_list[i * d + j]);
            if (j < d - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
/*after prnting - free all memory*/
    free(final_centroids_list);
    free(data);
    return 0;
}
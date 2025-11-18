//made by nomespaladin

//include required libs.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define MAX_PASSWORD_LENGTH 10
#define CHARSET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define CHARSET_SIZE 62

//global variables that indicates the password was found 
int password_found = 0;
char found_password[MAX_PASSWORD_LENGTH + 1];

/**
 * generate every possible combination with length 'length' and compares with the target password
 * @param target: target password to find
 * @param length: actual length to generate
 * @param max_length: max password length
 */

void bruteforce_recursive(const char *target, char *current, int position, int length) {

    if (password_found) return;

    if (position == length) {
        current[position] = '\0';

        //thread-safe with critical clausule
        if (strcmp(current, target) == 0) {
            #pragma omp critical
            {
                if (!password_found) {
                    password_found = 1;
                    strcpy(found_password, current);
                }
            }
        }
        return;
    }

    //generate every possible combination
    for (int i = 0; i < CHARSET_SIZE && !password_found; i++) {
        current[position] = CHARSET[i];
        bruteforce_recursive(target, current, position + 1, length);
    }
}

/**
 * main parallelized bruteforce function with OpenMP
 * @param target: target passwords to gather
 * @param max_length: max length to try bruteforce
 * @return: time elapsed in seconds
 */

double bruteforce_parallel(const char *target, int max_length) {
    double start_time = omp_get_wtime();
    int target_length = strlen(target);

    printf("Starting parallelized search with OpenMP...\n");
    printf("Target password (length %d)\n", target_length);
    printf("Available threads: %d\n\n", omp_get_max_threads());

    //iterate for every possible length
    for (int len = 1; len <= target_length && !password_found; len++) {
        printf("Trying passwords with length %d...\n", len);

        //parallelize the first tree searching level
        #pragma omp parallel
        {
            char current[MAX_PASSWORD_LENGTH + 1];

            //each thread start with a different character using schedule clausule with nowait
            #pragma omp for schedule(dynamic) nowait
            for (int i = 0; i < CHARSET_SIZE; i++) {
                if (!password_found) {
                    current[0] = CHARSET[i];
                    bruteforce_recursive(target, current, 1, len);
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    return end_time - start_time;
}


//calculates the aprox. combination number for a given length
unsigned long long calculate_combinations(int length) {
    unsigned long long combinations = 1;
    for (int i = 0; i < length; i++) {
        combinations *= CHARSET_SIZE;
    }
    return combinations;
}

int main(int argc, char *argv[]) {
    char target_password[MAX_PASSWORD_LENGTH + 1];
    int num_threads = 4; //default number of threads

    printf("==============================================\n");
    printf("  PARALLEL BRUTEFORCE WITH OpenMP\n");
    printf("==============================================\n\n");

    //config. number of threads as an argument
    if (argc > 1) {
        num_threads = atoi(argv[1]);
        if (num_threads < 1) num_threads = 1;
    }

    //config the password from an argument or use default
    if (argc > 2) {
        strncpy(target_password, argv[2], MAX_PASSWORD_LENGTH);
        target_password[MAX_PASSWORD_LENGTH] = '\0';
    } else {
        //default example password
        strcpy(target_password, "abc123");
    }

    //OMP config.
    omp_set_num_threads(num_threads);

    //data display
    printf("Config.:\n");
    printf("-> Char. group: %s\n", CHARSET);
    printf("-> Group size: %d caracteres\n", CHARSET_SIZE);
    printf("-> Number of threads: %d\n", num_threads);
    printf("-> Target password length: %d\n\n", (int)strlen(target_password));

    //show complexity
    unsigned long long total_combinations = 0;
    for (int i = 1; i <= strlen(target_password); i++) {
        total_combinations += calculate_combinations(i);
    }
    printf("Total combinations to try out: %llu\n\n", total_combinations);

    //start bruteforce 
    double elapsed_time = bruteforce_parallel(target_password, strlen(target_password));

    //show results
    printf("\n==============================================\n");
    if (password_found) {
        printf("PASSWORD FOUND: %s\n", found_password);
        printf("Elapsed time: %.6f seconds\n", elapsed_time);

        //calculate velocity/time
        double speed = total_combinations / elapsed_time;
        printf("Aprox. Velocity: %.2f attempts/second\n", speed);
    } else {
        printf("PASSWORD NOT FOUND\n");
        printf("Elapsed Time: %.6f seconds\n", elapsed_time);
    }
    printf("==============================================\n");

    return password_found ? 0 : 1;
}

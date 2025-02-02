#include <stdio.h>

#include "ccsv.h"

int test1_1()
{
    CsvStruct csv;
    char *filename = "test/test_data/test1_1.csv";
    csv_read(filename, &csv);
    if (csv.data ==NULL) {
        fprintf(stderr, "test1_1 error occured.\n");
        return 1;
    }
    debug_csv_stdout(&csv);
    memory_free_csv_struct(&csv);
    return 0;
}

int test2_1()
{
    CsvStruct csv;
    char *filename = "test/test_data/test2_1.csv";
    csv_read(filename, &csv);
    if (csv.data ==NULL) {
        fprintf(stderr, "test2_1 error occured.\n");
        return 1;
    }
    printf("debug1\n");
    debug_csv_stdout(&csv);
    memory_free_csv_struct(&csv);
    return 0;
}


int main()
{
    if (test1_1() == 1) {
        fprintf(stderr, "test1_1 failed.\n");
    }
    if (test2_1() == 1) {
        fprintf(stderr, "test2_1 failed.\n");
    }
}
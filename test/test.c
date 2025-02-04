/**
 * @file test.c
 * @brief test code for ccsv.c
 * @details It is recommended to change the following constants when executing test code:
 * #define DEFAULT_LINE_SIZE 150  
 * #define DEFAULT_WORD_SIZE 20  
 * #define DEFAULT_COLUMN_NUM 5  
 * #define DEFAULT_ROW_NUM 5 
 */

#include <stdio.h>

#include "ccsv.h"

/**
 * @brief  validata the parsing logic for CSV format
 * @return success(0) or failure(1) flag
 */
int test1_1()
{
    DEBUG_PRINT("test1_1 start");
    CsvStruct csv;
    char *filename = "test/test_data/test1_1.csv";
    csv_read(filename, &csv);
    if (csv.data ==NULL) {
        fprintf(stderr, "test1_1 error occured.\n");
        return 1;
    }
    debug_csv_stdout(&csv);
    memory_free_csv_struct(&csv);
    DEBUG_PRINT("test1_1 end");
    return 0;
}

/**
 * @brief validate the memory reallocation for CSV row data.
 * @return success(0) or failure(1) flag
 */
int test2_1()
{
    DEBUG_PRINT("test2_1 start");
    CsvStruct csv;
    char *filename = "test/test_data/test2_1.csv";
    csv_read(filename, &csv);
    if (csv.data ==NULL) {
        fprintf(stderr, "test2_1 error occured.\n");
        return 1;
    }
    debug_csv_stdout(&csv);
    memory_free_csv_struct(&csv);
    DEBUG_PRINT("test2_1 end");
    return 0;
}

/**
 * @brief validate the memory reallocation for elements of a CSV row.
 * @return success(0) or failure(1) flag
 */
int test2_2()
{
    DEBUG_PRINT("test2_2 start");
    CsvStruct csv;
    char *filename = "test/test_data/test2_2.csv";
    csv_read(filename, &csv);
    if (csv.data ==NULL) {
        DEBUG_PRINT("test2_2 failed.\n");
        return 1;
    }
    debug_csv_stdout(&csv);
    memory_free_csv_struct(&csv);
    DEBUG_PRINT("test2_2 end");
    return 0;
}

/**
 * @brief validate the memory reallocation for whole CSV data.
 * @return success(0) or failure(1) flag
 */
int test2_3()
{
    DEBUG_PRINT("test2_3 start");
    CsvStruct csv;
    char *filename = "test/test_data/test2_3.csv";
    csv_read(filename, &csv);
    if (csv.data ==NULL) {
        DEBUG_PRINT("test2_3 failed.\n");
        return 1;
    }
    debug_csv_stdout(&csv);
    memory_free_csv_struct(&csv);
    DEBUG_PRINT("test2_3 end");
    return 0;
}

int main()
{
    if (test1_1() == 1) {
        DEBUG_PRINT("test1_1 failed.\n");
    }
    if (test2_1() == 1) {
        DEBUG_PRINT("test2_1 failed.\n");
    }
    if (test2_2() == 1) {
        DEBUG_PRINT("test2_2 failed.\n");
    }
    if (test2_3() == 1) {
        DEBUG_PRINT("test2_3 failed.\n");
    }
}
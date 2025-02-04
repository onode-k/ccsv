/**
 * @file ccsv.h 
 * @brief ccsv main header file
 */
#ifndef CCSV
#define CCSV

#define DEFAULT_LINE_SIZE 150 /* The buffer size for reading a single line from a CSV file. */
#define DEFAULT_WORD_SIZE 20  /* The buffer size for storing a token. */
#define DEFAULT_COLUMN_NUM 5  /* The buffer size for storing tokens a single line. */
#define DEFAULT_ROW_NUM 5     /* The buffer size for storing lines of data from a CSV file. */

//#define DEBUG /* debugging flag */

/**
 * @brief A structure to represent a CSV data.
 */
typedef struct _csv_struct {
    char ***data; /**< a pointer to a pointer to a pointer to a char, used to store CSV data in a 3D array-like structure */
    int rows;     /**< The number of CSV rows in the CSV file. */
    int columns;  /**< The number of CSV columns in the CSV file.*/
} CsvStruct;

/**
 * @brief prints the CsvStruct structure to standard output for debug.
 * @param [in] csv : pointer to CsvStruct structure.
 * @return This function does not return a value.
 */
void debug_csv_stdout(CsvStruct *csv);

#ifdef DEBUG
    /** @def
     * A macro for printf-based debugging.
     */
    #define DEBUG_PRINT(msg, ...) \
        fprintf(stdout, "[DEBUG] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(msg, ...)
#endif

/**
 * @brief Function to deallocate memory for a CsvStruct structure.
 * @param [in] csv : pointer to CsvStruct structure.
 * @return This function does not return a value.
 */
void memory_free_csv_struct(CsvStruct *csv);

/**
 * @brief Function to initialize a CSVStruct structure.
 * @param [in] csv : Pointer to CsvStruct structure.
 * @return result
 * @retval 0 : Failed
 * @retval 1 : Success
 */
int initialize_CsvStruct(CsvStruct *csv);

/**
 * @brief Function to read a CSV file line by line and store the data in CsvStruct structure.
 * @param [in] filename : The path to CSV file, either an absolute or relative path.
 * @param [in] csv : Pointer to CsvStruct structure. which can be either initialized or uninitialized
 * @return This function does not return a value.
 */
void csv_read(char *filename, CsvStruct *csv);

#endif
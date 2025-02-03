#ifndef CCSV
#define CCSV

#define DEFAULT_LINE_SIZE 150
#define DEFAULT_WORD_SIZE 20
#define DEFAULT_COLUMN_NUM 5
#define DEFAULT_ROW_NUM 5

#define DEBUG /* FIXME */

typedef struct _csv_struct {
    char ***data;
    int rows;
    int columns;
} CsvStruct;

#ifdef DEBUG
void debug_csv_stdout(CsvStruct *csv);

#define DEBUG_PRINT(msg, ...) \
    fprintf(stdout, "[DEBUG] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

void memory_free_csv_struct(CsvStruct *csv);

int initialize_CsvStruct(CsvStruct *csv);

void csv_read(char *filename, CsvStruct *csv);

char **tokenizer(char *line, int *cols);

#endif
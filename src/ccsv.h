#ifndef CCSV
#define CCSV

#define DEFAULT_LINE_SIZE 2048
#define DEFAULT_WORD_SIZE 50
#define DEFAULT_COLUMN_NUM 50
#define DEFAULT_ROW_NUM 50

#define DEBUG /* FIXME */

typedef struct _csv_struct {
    char ***data;
    int rows;
    int columns;
} CsvStruct;

#ifdef DEBUG
void debug_csv_stdout(CsvStruct *csv);
#endif

void memory_free_csv_struct(CsvStruct *csv);

int initialize_CsvStruct(CsvStruct *csv);

void csv_read(char *filename, CsvStruct *csv);

char **tokenizer(char *line, int *cols);

#endif
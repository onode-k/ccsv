#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"

#ifdef DEBUG
static void debug_csv_stdout(CsvStruct *csv)
{
    for (int i=0; i < csv->rows; i++) {
        for (int j=0; j < csv->columns; j++) {
            printf("%s, ", csv->data[i][j]);
        }
        printf("\n");
    }
}
#endif

void memory_free_csv_struct(CsvStruct *csv)
{
    for (int i=0; i < csv->rows; i++) {
        for (int j=0; j < csv->columns; j++) {
            free(csv->data[i][j]);
        }
        free(csv->data[i]);
        csv->data[i] = NULL;
    }
    csv->data = NULL;
    csv->columns = 0;
    csv->rows = 0;
    csv = NULL;
}

static void memory_free_csv_line(char *element_buffer, char **row_ptr, unsigned int column_num) {
    if (element_buffer != NULL) {
        free(element_buffer);
    }
    for (int i=0; i<column_num; i++) {
        free(row_ptr[i]);
        row_ptr[i] = NULL;
    }
    free(row_ptr);
}

int initialize_CsvStruct(CsvStruct *csv)
{
    /* CsvStruct構造体の初期化 */
    csv->data = malloc(DEFAULT_ROW_NUM * sizeof(char **));
    if (csv->data == NULL) {
        csv->columns = 0;
        csv->rows = 0;
        return 0;
    }
    csv->columns = 0;
    csv->rows =0;
    return 1;
}

void csv_read(char *filename, CsvStruct *csv)
{
    FILE *input_file;
    char *line_buffer;
    char **tokenized_line;
    unsigned int cols;

    /* ファイル読み込み */
    input_file = fopen(filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        csv->data = NULL;
        csv->columns = 0;
        csv->rows = 0;
        return;
    }
    /* CsvStruct構造体の初期化 */
    if (!(initialize_CsvStruct(csv))) {
        return;
    }
    /* CSVファイルの行データを格納するメモリを確保 */
    line_buffer = malloc(DEFAULT_LINE_SIZE);
    if (line_buffer == NULL) {
        /* TODO: 終了処理 */
        csv->data = NULL;
        csv->columns = 0;
        csv->rows = 0;
        return;
    }
    unsigned int rows = 0;
    unsigned int row_index;
    while (fgets(line_buffer, DEFAULT_LINE_SIZE, input_file) != NULL) {
        ++rows;
        size_t word_num = strlen(line_buffer);      /* 改行文字を含みヌル文字を含まない文字数 */
        unsigned int line_break_pos = word_num - 1; /* 改行文字のインデックスを取得 */
        if ((line_buffer[line_break_pos] != '\n') && (!feof(input_file))) {
            /* バッファが溢れた場合拡張を行う */
            while (1) {
                char *tmp = (char*)realloc(line_buffer, (word_num + 1 + DEFAULT_LINE_SIZE)); /* 現在のバッファサイズからDEFAULT_LINE_SIZE分足し込む */
                if (tmp == NULL) {
                    free(line_buffer);
                    line_buffer = NULL;
                    memory_free_csv_struct(csv);
                    return;
                    /* TODO: メモリ解放処理 */
                }
                line_buffer = tmp;
                unsigned int insert_index = strlen(line_buffer);
                fgets(line_buffer+insert_index, DEFAULT_LINE_SIZE, input_file);
                if ((line_buffer[strlen(line_buffer)-1] == '\n') || (feof(input_file))) {
                    break;
                }
            }
        }
        tokenized_line = tokenizer(line_buffer, &cols);
        if (tokenized_line == NULL) {
            memory_free_csv_struct(csv);
            break;
        }
        /* csv_structに格納していく */
        if (rows >= DEFAULT_ROW_NUM) {
            char ***tmp = (char ***)realloc(csv->data, sizeof(char **) * (rows + DEFAULT_ROW_NUM));
            if (tmp == NULL) {
                memory_free_csv_struct(csv);
                return;
            }
            csv->data = tmp;
        }
        row_index = rows - 1;
        csv->data[row_index] = tokenized_line;
        csv->columns = cols;
        csv->rows = rows;
    }
    /* 過剰に確保したメモリ領域を解放する */
    char ***tmp = realloc(csv->data, rows * sizeof(char **));
    if (tmp == NULL) {
        memory_free_csv_struct(csv);
        return;
    }
    csv->data = tmp;
    csv->rows = rows;
    csv->columns = cols;
    fclose(input_file);
    return;
}
char **tokenizer(char *line, unsigned int *cols)
{
    char         *current_ptr;
    char         **row_ptr;
    unsigned int column_num;
    current_ptr    = line;
    /* 行の要素のポインタを格納するメモリ領域を確保 */
    row_ptr = (char **)malloc(DEFAULT_COLUMN_NUM * sizeof(char*));
    if (row_ptr == NULL) {
        fprintf(stderr, "memory allocation failed.\n");
        return row_ptr;
    }
    column_num = 0;
    while (*current_ptr != '\0') {
        char         *token_tail_ptr = NULL;
        unsigned int word_index = 0;
        unsigned int curr_word_limit_size = DEFAULT_WORD_SIZE - 1; /* ヌル文字分-1 */
        char         *element_buffer = (char*)malloc(DEFAULT_WORD_SIZE); /* 要素を格納するメモリの確保 */
        
        if (element_buffer == NULL) {
            /* エラーハンドリングとメモリの解放処理 */
            fprintf(stderr, "memory allocation failed.\n");
            memory_free_csv_line(NULL, row_ptr, column_num);
            row_ptr = NULL;
            break;
        }
        /* ヌル文字で初期化 */
        memset(element_buffer, '\0', DEFAULT_WORD_SIZE);
        if (column_num >= DEFAULT_COLUMN_NUM) {
            /* カラム数がデフォルトサイズより大きくなった場合メモリを拡張する */
            size_t new_length = column_num + DEFAULT_COLUMN_NUM;
            char **tmp = (char **)realloc(row_ptr, new_length);
            if (tmp == NULL) {
                fprintf(stderr, "memory allocation failed.\n");
                /* 既に確保した要素のメモリを解放 */
                memory_free_csv_line(element_buffer, row_ptr, column_num);
                row_ptr = NULL;
                element_buffer = NULL;
                break;
            }
            row_ptr = tmp;
        }
        if ((*current_ptr != '"')) {
            fprintf(stderr, "csv parse error.\n");
            memory_free_csv_line(element_buffer, row_ptr, column_num);
            element_buffer = NULL;
            row_ptr = NULL;
            break;
        }
        ++current_ptr;
        while (token_tail_ptr == NULL) {
            /* 要素の文字列を格納するメモリ領域の動的確保 */
            if ((strlen(element_buffer)) >= (curr_word_limit_size)) { 
                /* ヌル文字が上書きされる前にバッファの拡張を行う(strlen()の動作が\0が存在しない場合の動作は未定義のため) */
                char *tmp = (char*)realloc(element_buffer, strlen(element_buffer) + DEFAULT_WORD_SIZE);
                if (tmp == NULL) {
                    fprintf(stderr, "memory allocation failed.\n");
                    /* エラー処理とメモリの解放 */
                    memory_free_csv_line(element_buffer, row_ptr, column_num);
                    element_buffer = NULL;
                    row_ptr = NULL;
                    *current_ptr = '\0'; /* ループから抜けるために\0を設定 */
                    break;
                }
                element_buffer = tmp;
                curr_word_limit_size = strlen(element_buffer); /* ヌル文字文+1する */
            }
            /* パース処理 */
            if ((*current_ptr != '"')) {
                /* 引用符および区切り文字以外の処理 */
                element_buffer[word_index] = *current_ptr;
                ++word_index;
                ++current_ptr;
            }
            else if (*current_ptr == '"') {
                /* 引用符の処理 */
                if ((*(current_ptr+1) == '"')) {
                    /* ダブルクオートのエスケープ対応 */
                    while (*(current_ptr+1) == '"') {
                        ++current_ptr; /* エスケープ文字をスキップ */
                        element_buffer[word_index] = *current_ptr;
                        ++word_index;
                    }
                    ++current_ptr;
                    continue;
                }
                
                if ((*(current_ptr+1) == ',') && ((*(current_ptr+2) == '"'))) {
                    token_tail_ptr = current_ptr;
                    current_ptr += 2; /* 次の要素の引用符にポインタを進める */
                    word_index = 0;
                    continue;
                }
                else if (*(current_ptr+1) == '\0') {
                    token_tail_ptr = current_ptr;
                    ++current_ptr;
                    word_index = 0;
                }
                else {
                    fprintf(stderr, "csv format error.\n");
                    memory_free_csv_line(element_buffer, row_ptr, column_num);
                    element_buffer = NULL;
                    row_ptr = NULL;
                    *current_ptr = '\0'; /* ループから抜けるために\0を設定 */
                    break;
                }
            }
        }
        /* 余分なメモリ領域を除去 */
        unsigned int token_size = strlen(element_buffer) + 1;
        char *tmp = realloc(element_buffer, token_size);
        if (tmp == NULL) {
            memory_free_csv_line(element_buffer, row_ptr, column_num);
            element_buffer = NULL;
            row_ptr = NULL;
            *cols = 0;
            return row_ptr;
        }
        element_buffer = tmp;
        row_ptr[column_num] = element_buffer;
        
        ++column_num;
    }
    *cols = column_num;
    return row_ptr;
}

int main(int argc, char *argv[])
{
    char *filename;
    CsvStruct csv;
    if (argc != 2) {
        perror("Error: Insufficient command-line arguments provided.");
        return 1;
    }
    filename = argv[1];
    csv_read(filename, &csv);
    printf("%d\n", csv.columns);
    printf("%d\n", csv.rows);
    debug_csv_stdout(&csv);
    return 0;
}
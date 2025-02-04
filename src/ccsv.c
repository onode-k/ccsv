/**
 * @file ccsv.c
 * @brief This file contains main logic implementation of the ccsv library
 */
#ifndef INTERNAL_HELPERS_H
#define INTERNAL_HELPERS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccsv.h"
#endif

/**
 * @brief private function related to memory deallocation used in internal processes.
 * @param [in] element_buffer : Pointer to a char where the CSV token is stored.
 * @param [in] row_ptr : Pointer to pointer to a char that stores pointers to tokens of a CSV row.
 * @param [in] column_num : The number of columns in the CSV file.
 * @return This function does not return a value.
 */
static void memory_free_csv_line(char *element_buffer, char **row_ptr, int column_num);

/**
 * @brief A private function to split a line of CSV data into token.
 * @param [in] line : A pointer to char array containing a single line of CSV data from the CSV file.
 * @param [out] cols : A pointer to an int that stores the number of columns.
 * @return A pointer to pointer to a char, which stores pointer to the tokenized elements of the CSV.
 *         Returns NULL if parsing failed.
 */
static char **tokenizer(char *line, int *cols);




void debug_csv_stdout(CsvStruct *csv)
{
    printf("column: %d\n", csv->columns);
    printf("row: %d\n", csv->rows);
    for (int i=0; i < csv->rows; i++) {
        for (int j=0; j < csv->columns; j++) {
            printf("%s, ", csv->data[i][j]);
        }
        printf("\n");
    }
}

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

static void memory_free_csv_line(char *element_buffer, char **row_ptr, int column_num) {
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
    int cols;

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

    int rows = 0;
    int row_index;
    size_t word_num;
    int row_thresh = DEFAULT_ROW_NUM;
    while (fgets(line_buffer, DEFAULT_LINE_SIZE, input_file) != NULL) {
        ++rows;
        word_num = strlen(line_buffer);      /* 改行文字を含みヌル文字を含まない文字数 */
        int line_break_pos = word_num - 1; /* 改行文字のインデックスを取得 */
        if ((line_buffer[line_break_pos] != '\n') && (!feof(input_file))) {
            /* バッファが溢れた場合拡張を行う */
            while (1) {
                DEBUG_PRINT("strlen(line_buffer): %ld line_buffer: %s\n", strlen(line_buffer), line_buffer);
                char *tmp = (char*)realloc(line_buffer, (word_num + 1 + DEFAULT_LINE_SIZE)); /* 現在のバッファサイズからDEFAULT_LINE_SIZE分足し込む(ヌル文字分+1) */
                if (tmp == NULL) {
                    free(line_buffer);
                    line_buffer = NULL;
                    memory_free_csv_struct(csv);
                    return;
                }
                line_buffer = tmp;
                int insert_index = strlen(line_buffer);
                fgets(line_buffer+insert_index, DEFAULT_LINE_SIZE, input_file);
                // printf("line_buffer: %s\n", line_buffer);
                if ((line_buffer[strlen(line_buffer)-1] == '\n') || (feof(input_file))) {
                    break;
                }
                word_num = strlen(line_buffer);
            }
        }
        DEBUG_PRINT("line data: %s\n", line_buffer);
        tokenized_line = tokenizer(line_buffer, &cols);
        if (tokenized_line == NULL) {
            memory_free_csv_struct(csv);
            break;
        }
        /* csv_structに格納していく */
        if (rows >= row_thresh) {
            DEBUG_PRINT("occured memory reallocation for `csv->data`. rows:%d thresh: %d\n", rows, row_thresh);
            char ***tmp = (char ***)realloc(csv->data, sizeof(char **) * (rows + DEFAULT_ROW_NUM));
            if (tmp == NULL) {
                memory_free_csv_struct(csv);
                return;
            }
            csv->data = tmp;
            row_thresh = rows + DEFAULT_COLUMN_NUM;
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

static char **tokenizer(char *line, int *cols)
{
    char *current_ptr;
    char **row_ptr;
    int  column_num;
    int  word_index;
    int  close_flag;          /* パース用フラグ */
    int  parse_complete_flag; /* パース用フラグ */
    int column_num_thresh = DEFAULT_COLUMN_NUM;


    current_ptr = line;
    /* 行の要素のポインタを格納するメモリ領域を確保 */
    row_ptr = (char **)malloc(DEFAULT_COLUMN_NUM * sizeof(char *));
    if (row_ptr == NULL) {
        fprintf(stderr, "memory allocation failed.\n");
        return row_ptr;
    }

    column_num = 0;
    while (*current_ptr != '\0') {
        int  curr_word_limit_size = DEFAULT_WORD_SIZE;
        char *element_buffer = (char*)malloc(DEFAULT_WORD_SIZE); /* 要素を格納するメモリの確保 */
        
        if (element_buffer == NULL) {
            /* エラーハンドリングとメモリの解放処理 */
            fprintf(stderr, "memory allocation failed.\n");
            memory_free_csv_line(NULL, row_ptr, column_num);
            row_ptr = NULL;
            break;
        }
        /* ヌル文字で初期化 */
        memset(element_buffer, '\0', DEFAULT_WORD_SIZE);
        if (column_num >= column_num_thresh) {
            column_num_thresh += DEFAULT_COLUMN_NUM;
            /* カラム数がデフォルトサイズより大きくなった場合メモリを拡張する */
            char **tmp = (char **)realloc(row_ptr, column_num_thresh * sizeof(char *));
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

        close_flag = 0;
        parse_complete_flag = 0;
        word_index = 0;
        while (!parse_complete_flag && *current_ptr != '\0') {
            /* 要素の文字列を格納するメモリ領域の動的確保 */
            if ((strlen(element_buffer)+1) >= curr_word_limit_size) { /* null文字分+1する */
                /* ヌル文字が上書きされる前にバッファの拡張を行う(strlen()の動作が\0が存在しない場合の動作は未定義のため) */
                DEBUG_PRINT("strlen(element_buffer):%ld element_buffer:%s\n", strlen(element_buffer), element_buffer);
                char *tmp = (char*)realloc(element_buffer, (strlen(element_buffer) + 1 + DEFAULT_WORD_SIZE)); /* null文字分+1 */
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
                curr_word_limit_size = strlen(element_buffer) + 1 + DEFAULT_WORD_SIZE;
                memset(element_buffer+strlen(element_buffer)+1, '\0', DEFAULT_WORD_SIZE);
            }
            /* パース処理 */
            if ((*current_ptr != '"') && (*current_ptr != ',')) {
                /* 引用符および区切り文字以外の処理 */
                element_buffer[word_index] = *current_ptr;
                ++word_index;
                ++current_ptr;
            }
            else if (*current_ptr == '"') {
                /* 引用符の処理 */
                if (close_flag) {
                    /* エスケープされた引用符の処理 */
                    element_buffer[word_index] = *current_ptr;
                    close_flag = 0;
                    ++word_index;
                    ++current_ptr;
                }
                else {
                    /* closeflagがたっていなかった場合の処理 */
                    close_flag = 1;
                    ++current_ptr;
                }
            }
            else if (*current_ptr == ',') {
                if (close_flag) {
                    parse_complete_flag = 1;
                    ++current_ptr;
                }
                else {
                    element_buffer[word_index] = *current_ptr;
                    ++word_index;
                    ++current_ptr;
                }
            }
        }
        /* 余分なメモリ領域を除去 */
        int token_size = strlen(element_buffer) + 1;
        char *tmp = realloc(element_buffer, token_size);
        if (tmp == NULL) {
            fprintf(stderr, "memory allocation failed\n");
            memory_free_csv_line(element_buffer, row_ptr, column_num);
            element_buffer = NULL;
            row_ptr = NULL;
            *cols = 0;
            return row_ptr;
        }
        element_buffer = tmp;
        DEBUG_PRINT("parsed token:%s\n", element_buffer);
        row_ptr[column_num] = element_buffer;

        ++column_num;
    }
    *cols = column_num;
    return row_ptr;
}
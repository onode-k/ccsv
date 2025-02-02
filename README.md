## テスト観点

## パーサ

1. 要素内にエスケープ文字(ダブルクオート)があった場合  
2. 要素内にカンマがあった場合


## メモリ管理

1. csvの1行分の文字列を格納するメモリ領域`line_buffer(char *)`の再確保(DEFAULT_LINE_SIZE 2048を超える場合)  
2. 行の要素のポインタを格納するメモリ領域`row_ptr(char **)`の再確保(DEFAULT_COLUMN_NUM 10を超える場合)  
3. 要素を格納するメモリ領域`element_buffer(char *)`の再確保(DEFAULT_WORD_SIZE 50を超える場合)  
4. 行データを格納するメモリ領域(`CsvStruct.data(char ***)`)の再確保(DEFAULT_ROW_NUM 50を超える場合)

## TODO

- マルチバイト対応  
- 引用符をシングルクオートにも対応  
- アルゴリズムをシングルスキャンに変更  
- Pythonラッパーの作成  
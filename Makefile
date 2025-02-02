# コンパイラとフラグ
CC = gcc
CFLAGS = -Wall -Wextra
AR = ar
ARFLAGS = rcs

# ライブラリ関連
LIB_NAME = ccsv.a
LIB_DIR = src
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJS = $(LIB_SRCS:.c=.o)

# テスト関連
TEST_DIR = test
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_EXEC = test_runner

# デフォルトターゲット
all: $(LIB_NAME) $(TEST_EXEC)

# ライブラリのビルド
$(LIB_NAME): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

# テストコードのビルド
$(TEST_EXEC): $(TEST_OBJS) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $^

# オブジェクトファイルの生成ルール
%.o: %.c
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $< -o $@

# クリーンアップ
clean:
	rm -f $(LIB_DIR)/*.o $(TEST_DIR)/*.o $(LIB_NAME) $(TEST_EXEC)

# テスト実行
test: $(TEST_EXEC)
	./$(TEST_EXEC)
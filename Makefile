# 컴파일러와 플래그 설정
CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = -lcheck

# 디렉토리 설정
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BIN_DIR = bin
BIN_TEST_DIR = bin/tests

# 실행 파일의 이름
MAIN_TARGET = $(BIN_DIR)/main

TEST_TARGETS = $(BIN_TEST_DIR)/test_Huffman_node



# 소스 파일(메인 제외)과 객체 파일
SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

# 메인 파일 객체
MAIN_OBJ = $(SRC_DIR)/main.o

# 테스트 파일과 객체 파일
TEST_SRC_FILES = $(wildcard $(TEST_DIR)/test_*.c)
TEST_OBJ_FILES = $(TEST_SRC_FILES:$(TEST_DIR)/%.c=$(TEST_DIR)/%.o)

# 기본 빌드 타겟
all: $(MAIN_TARGET) $(TEST_TARGETS)

# 메인 프로그램 빌드
$(MAIN_TARGET): $(OBJ_FILES) $(MAIN_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# 테스트 코드 빌드
$(BIN_TEST_DIR)/test_%: $(OBJ_FILES) $(TEST_DIR)/test_%.o
	mkdir -p $(BIN_TEST_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 개별 .o 파일 생성 규칙
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# 실행
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# 테스트 실행
test: $(TEST_TARGETS)
	@for test in $(TEST_TARGETS); do \
	    echo "Running $$test..."; \
	    ./$$test; \
	done

clean:
	rm -rf $(BIN_DIR)   
	rm -f $(SRC_DIR)/*.o   
	rm -f $(TEST_DIR)/*.o  

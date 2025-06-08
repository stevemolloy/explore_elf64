CC = clang
CFLAGS = -Wall -Wextra -Wconversion -std=c99 -Wpedantic -ggdb
CLIBS =

SRC = src
BIN = bin
OBJ = obj

EXE_NAME = main
EXE = $(BIN)/$(EXE_NAME)

TEST_TGT_NAME = test_target
TEST_TGT_DIR = testfiles
TEST_TGT = $(TEST_TGT_DIR)/$(TEST_TGT_NAME)

all: $(EXE) $(TEST_TGT)

$(EXE): $(OBJ)/$(EXE_NAME).o
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

$(OBJ)/$(EXE_NAME).o: $(SRC)/$(EXE_NAME).c
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c -o $@ $^

$(TEST_TGT): $(OBJ)/$(TEST_TGT_NAME).o
	@mkdir -p $(TEST_TGT_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

$(OBJ)/$(TEST_TGT_NAME).o: $(SRC)/$(TEST_TGT_NAME).c
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c -o $@ $^

run: $(EXE) $(TEST_TGT)
	$(EXE)


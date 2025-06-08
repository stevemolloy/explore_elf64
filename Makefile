CC = clang
CFLAGS = -Wall -Wextra -std=c99 -Wpedantic -ggdb
CLIBS = 

SRC = src
BIN = bin
OBJ = obj

EXE_NAME = main
EXE = $(BIN)/$(EXE_NAME)

TEST_TGT_NAME = test_target
TEST_TGT = $(BIN)/$(TEST_TGT_NAME)

all: $(EXE) $(TEST_TGT)

$(EXE): $(OBJ)/$(EXE_NAME).o
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

$(OBJ)/$(EXE_NAME).o: $(SRC)/$(EXE_NAME).c
	@mkdir -p $(OBJ)
	$(CC) $(FLAGS) -c -o $@ $^

$(TEST_TGT): $(OBJ)/$(TEST_TGT_NAME).o
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

$(OBJ)/$(TEST_TGT_NAME).o: $(SRC)/$(TEST_TGT_NAME).c
	@mkdir -p $(OBJ)
	$(CC) $(FLAGS) -c -o $@ $^


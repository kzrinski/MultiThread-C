CFLAGS = -Wall -pedantic -std=c99 -Iinclude
CC = gcc
SRC = ./source/
BUILD = ./build/
INC = ./include/
OBJ = $(BUILD)main.o $(BUILD)log.o $(BUILD)fileIO.o $(BUILD)Queue.o
EXEC = scheduler
DBG = $(SRC)*.c

$(EXEC) : $(OBJ)
	@$(CC) $(OBJ) -lpthread -o $(EXEC)
	@echo "[LINKING] $(OBJ)"

$(BUILD)main.o : $(SRC)main.c $(INC)*.h
	@$(CC) -c $(SRC)main.c $(CFLAGS)
	@mv main.o build
	@echo "[COMPILING] main.c"

$(BUILD)fileIO.o : $(SRC)fileIO.c $(INC)fileIO.h $(INC)macros.h $(INC)Queue.h
	@$(CC) -c $(SRC)fileIO.c $(CFLAGS)
	@mv fileIO.o build
	@echo "[COMPILING] fileIO.c"

$(BUILD)log.o : $(SRC)log.c $(INC)log.h $(INC)macros.h $(INC)fileIO.h
	@$(CC) -c $(SRC)log.c $(CFLAGS)
	@mv log.o build
	@echo "[COMPILING] log.c"

$(BUILD)Queue.o : $(SRC)Queue.c $(INC)Queue.h
	@$(CC) -c $(SRC)Queue.c $(CFLAGS)
	@mv Queue.o build
	@echo "[COMPILING] Queue.c"

debug : $(OBJ)
	@$(CC) -c -g $(DBG) $(CFLAGS) -D DEBUG=1
	@echo "[COMPILING] Source files"
	@mv *.o build
	@$(CC) $(OBJ) -lpthread -o $(EXEC)
	@echo "[LINKING] Object files"
	@echo "[DONE] Compiled with DEBUG"

clean :
	@rm -f $(EXEC) $(OBJ)
	@echo "[CLEAN] Removing object and executable files"
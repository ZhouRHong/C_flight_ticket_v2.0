CC=gcc

TARGET=bin/project
LIB_NAME=./lib/libmy.so
MAIN_SOURCE=$(wildcard ./main/*.c)
INCLUDE_PATH=-I ./include/
LIB_PATH=-L ./lib -lmy
LTHREAD=-lpthread

C_SOURCE=$(wildcard ./src/*.c) 
OBJ=$(patsubst %.c,%.o,$(C_SOURCE))
%.o:%.c
	$(CC) $^ -o $@ -c -fPIC $(INCLUDE_PATH)
	
$(LIB_NAME):$(OBJ)
	$(CC) -fPIC -shared -o $@ $^

bin/project:$(MAIN_SOURCE)          
	$(CC) $^ -o $@ $(INCLUDE_PATH) $(LIB_PATH) $(LTHREAD)
	
.PHONY:clean
	
clean:
	$(RM) $(TARGET)
	$(RM) $(LIB_NAME) 
	$(RM) ./src/*.o

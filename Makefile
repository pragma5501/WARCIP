
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
UTL_DIR = ./src/util
INC_DIR = \
		./src \
		-I ./src/util \

GCCFLAGS += \
	-g \
	-fsanitize=address \


OBJ_SRC += \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/FTL.c \
	$(SRC_DIR)/ssd.c \
	$(SRC_DIR)/WARCIP.c \
	$(SRC_DIR)/dispatcher.c \
	$(SRC_DIR)/util/queue.c \
	
TARGET_OBJ =\
	$(patsubst %.c,%.o,$(OBJ_SRC))

all : my_pftl

my_pftl: $(TARGET_OBJ)
	@mkdir -p $(BIN_DIR)
	gcc -o    $(BIN_DIR)/$@ $(GCCFLAGS) $^ -I$(INC_DIR)

.c.o:
	gcc -c $< -o $@ -I$(INC_DIR)

-include $(OBJS:.o=.d)

clean:
	@rm -vf $(BIN_DIR)/*
	@rm -vf $(OBJ_DIR)/*.o
	@rm -vf $(SRC_DIR)/*.o
	@rm -vf $(UTL_DIR)/*.o
BUILD_DIR=build
SRC_DIR=src

$(BUILD_DIR)/2048: $(BUILD_DIR) $(SRC_DIR)/2048.c $(SRC_DIR)/2048.h
	cc -o $(BUILD_DIR)/2048 -lncurses $(SRC_DIR)/2048.c

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

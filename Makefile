SRC_DIR := ./libs
OBJ_DIR := ./libs/obj
BIN_DIR := ./bin

OUT_DIRS := $(OBJ_DIR) \
			$(BIN_DIR)

CPP_INC := -I./libs/utils \
		   -I./libs/core/flac

CPP_FLAGS := -std=c++11

LOCAL_SRC_DIR := ./libs/utils
LOCAL_SRC_FILES := $(wildcard $(LOCAL_SRC_DIR)/*.cpp)
SRC_FILES := $(LOCAL_SRC_FILES)
OBJ_FILES := $(patsubst $(LOCAL_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LOCAL_SRC_FILES))

LOCAL_SRC_DIR := ./libs/core/flac
LOCAL_SRC_FILES := $(wildcard $(LOCAL_SRC_DIR)/*.cpp)
SRC_FILES += $(LOCAL_SRC_FILES)
OBJ_FILES += $(patsubst $(LOCAL_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LOCAL_SRC_FILES))

test:
	echo $(SRC_FILES)
	echo $(OBJ_FILES)

directories:
	mkdir -p $(OUT_DIRS)

flac: $(OBJ_FILES)
	g++ $(CPP_FLAGS) $(CPP_INC) $@/demo.cpp -o $(BIN_DIR)/$@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp
	g++ $(CPP_FLAGS) $(CPP_INC) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/*/%.cpp
	g++ $(CPP_FLAGS) $(CPP_INC) -c -o $@ $<

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

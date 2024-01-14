TARGET=simple3d
OUTPUT_PATH=../../bin/emscripten/
LOCAL_INCLUDES=-I$(TBSDK_DIR)/include
LOCAL_LIBS=-L$(TBSDK_DIR)/lib/emscripten -ltb_framework -ltb_graphics -ltb_input -ltb_base
LOCAL_LIBS_DEBUG=-L$(TBSDK_DIR)/lib/emscripten -ltb_framework_d -ltb_graphics_d -ltb_input_d -ltb_base_d
SRCS=../../src/main.cpp\

include $(TBSDK_DIR)/build/common/emscripten/Makefile.common
	
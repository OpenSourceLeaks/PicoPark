TARGET=lua_simple
LOCAL_INCLUDES=$(TBSDK_DIR)/include $(TBSDK_DIR)/sdk/lua/5.3.1/include
LOCAL_LIBS=-L$(TBSDK_DIR)/lib/emscripten -L$(TBSDK_DIR)/sdk/lua/5.3.1/lib/emscripten -ltb_framework_cui -ltb_graphics -ltb_script_lua -ltb_base -llua
SRCS=../../src/main.cpp\
	 
include Makefile.common
	
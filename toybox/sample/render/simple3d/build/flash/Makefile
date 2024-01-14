TARGET=simple3d
SWF_SIZE=640x480

LIBS=\
    -ltb_framework \
    -ltb_graphics \
    -ltb_base

LIBS_DEBUG=\
    -ltb_framework_d \
    -ltb_graphics_d \
    -ltb_base_d

#    -ltest_d \

SRCS=\
    ../../src/main.cpp

#    ../../src/fwd.cpp \
    
ABCS=\
    VFS.abc \
    $(FLASCC)/usr/lib/AlcVFSZip.abc \

include ../../../../common/flash/Makefile.common

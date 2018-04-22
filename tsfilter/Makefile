# compiler to use: gcc for C program, define as g++ for C++
CC        = gcc
CXX       = g++
COMPILER  = $(CXX)

MAIN_HOME = ../

INCLUDES  = -I./include
INCLUDES += -I$(MAIN_HOME)include


# compile time flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS   += -g -Wall
CXXFLAGS  = $(CFLAGS)

# library paths in addition to /usr/lib
LDFLAGS   = -L./lib

# libraries to link into executable:
LIBS      = -lpthread

# source files
SRC_DIR   = src/
C_SRCS    = 
CXX_SRCS  = tsfilter_main.cpp devices.cpp

# object files: uses Suffix Replacement within a macro:
OBJ_DIR   = obj/
_OBJS     = $(C_SRCS:.c=.o)
_OBJS    += $(CXX_SRCS:.cpp=.o)
OBJS      = $(patsubst %,$(OBJ_DIR)%,$(_OBJS))

.PHONY: clean

# executable file
TARGET_DIR= bin/
TARGET    = $(TARGET_DIR)tsfilter


all: $(TARGET)

$(TARGET): $(OBJS) $(TARGET_DIR)
	$(COMPILER) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

$(TARGET_DIR):
	mkdir -p $@


# $@ Left side of ':'
# $< First item from the dependencies list
# $^ Right side of ':'

#.cpp.o:
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<

#.c.o:
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(OBJ_DIR):
	mkdir -p $@


clean:
	rm -f $(TARGET) $(OBJS)
	rmdir $(TARGET_DIR) 
	rmdir $(OBJ_DIR)


# References:
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
# https://stackoverflow.com/questions/1814270/gcc-g-option-to-place-all-object-files-into-separate-directory

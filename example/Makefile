CC      := g++ -std=c++11
CFLAGS  :=-Wall -O2

TARGET  := Demo
CLIBS   := -lNetLib -lCSLog -llog4cplus
INCLUDES:=-I ../include/
SRCS    :=$(wildcard *.cpp)
OBJS    :=$(patsubst %cpp, %o, $(SRCS))
all     : $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(CLIBS)

%.o:%.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) $^




clean:
	rm -rf *.o $(TARGET) $(OBJS)

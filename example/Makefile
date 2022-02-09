CC      := g++ -std=c++11
CFLAGS  :=-Wall -O2

TARGET_SERVER  := TestServer
TARGET_CLIENT  := TestClient
CLIBS   := -lNetLib -lCSLog -llog4cplus
INCLUDES:=-I ../include/
SRCS    :=$(wildcard *.cpp)
OBJS    :=$(patsubst %cpp, %o, $(SRCS))
all     : $(TARGET_SERVER) $(TARGET_CLIENT)
$(TARGET_SERVER): TestServer.o
	$(CC) $(CFLAGS) TestServer.o -o $@ $(CLIBS)

$(TARGET_CLIENT): TestClient.o
	$(CC) $(CFLAGS) TestClient.o -o $@ $(CLIBS)

%.o:%.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) $^




clean:
	rm -rf *.o $(TARGET_CLIENT) $(TARGET_SERVER) $(OBJS)

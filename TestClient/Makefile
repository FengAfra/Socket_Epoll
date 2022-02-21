CC      := g++ -std=c++11 -g
CFLAGS  :=-Wall -O0

TARGET_CLIENT  := TestClient
CLIBS   :=  -lCSLog -llog4cplus -lpthread
INCLUDES:=-I ../include/
SRCS    :=$(wildcard *.cpp)
OBJS    :=$(patsubst %cpp, %o, $(SRCS))
#all     : $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_CLIENT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(CLIBS)

%.o:%.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) $^




clean:
	rm -rf *.o $(TARGET_CLIENT) $(TARGET_SERVER) $(OBJS) *.log


CC = g++ -DLINUX -D_REENTRANT
FLAGS = -W -g

BASE_ROOT=../../../../../sdk/base/
JSON_ROOT=../../../../../3rd_party/sdk/json-c/
TINY_XML_ROOT=../../../../../3rd_party/sdk/tinyxml/
LIBCOMM_ROOT=../sdk/libcomm/
LOG_CLIENT_ROOT=../../log_svr/sdk/logclient/

INCLUDES=-I./ -I$(BASE_ROOT)/include/ -I$(JSON_ROOT)/include/ \
-I$(TINY_XML_ROOT)/include/ -I$(LIBCOMM_ROOT)/include/ -I$(LOG_CLIENT_ROOT)/include/

LIB_PATH=

SHARE_LIBS=$(LOG_CLIENT_ROOT)/lib/liblog.a $(LIBCOMM_ROOT)/lib/libcomm.a $(JSON_ROOT)/lib/libjson-c.a \
$(BASE_ROOT)/lib/libbase.a $(TINY_XML_ROOT)/lib/libtinyxml.a \
-lpthread -ldl -lrt -lz

TARGET=dip_router.bin

OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

obj/%.o: %.h
obj/%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(LIB_PATH) $(OBJS) $(SHARE_LIBS);
	cp $(TARGET)  ../../../release/bin/$(TARGET)

clean:
	rm obj/*.o
	rm $(TARGET)

init:
	mkdir -p obj
	

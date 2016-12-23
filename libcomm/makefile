
CC = g++ -DLINUX -D_REENTRANT
FLAGS = -W -g -fPIC

BASE_ROOT=../../../../../sdk/base/
JSON_ROOT=../../../../../3rd_party/sdk/json-c/
TINY_XML_ROOT=../../../../../3rd_party/sdk/tinyxml/

INCLUDES=-I./ -I$(BASE_ROOT)/include/ -I$(JSON_ROOT)/include/ \
-I$(TINY_XML_ROOT)/include/

LIB_PATH=

SHARE_LIBS=$(BASE_ROOT)/lib/libbase.a $(JSON_ROOT)/lib/libjson-c.a \
$(TINY_XML_ROOT)/lib/libtinyxml.a \
-lpthread -ldl -lrt -lz

SONAME=libcomm.so
TARGET=libcomm.so.1.0
S_TARGET=libcomm.a

ALL: $(TARGET) $(S_TARGET)

OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

obj/%.o: %.h
obj/%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@ 

$(TARGET): $(OBJS)
	$(CC) -shared -fPIC -Wl,-soname,$(SONAME) -o $(TARGET) $(OBJS) $(SHARE_LIBS)
	cp $(TARGET) ../sdk/libcomm/lib/
	cp $(TARGET) ../sdk/libcomm/lib/$(SONAME)
	cp *.h ../sdk/libcomm/include/libcomm/

$(S_TARGET): $(OBJS) 
	ar -r $(S_TARGET) obj/*.o 
	cp $(S_TARGET)  ../sdk/libcomm/lib

init:
	mkdir -p obj
	mkdir -p ../sdk/libcomm/include/libcomm
	mkdir -p ../sdk/libcomm/lib
	
clean:
	-rm obj/*.o
	-rm ./$(S_TARGET)
	-rm ./$(TARGET)	
	-rm ../sdk/libcomm/include/libcomm/*.h
	-rm ../sdk/libcomm/lib/$(S_TARGET)
	-rm ../sdk/libcomm/lib/$(TARGET)
	-rm ../sdk/libcomm/lib/$(SONAME)
	

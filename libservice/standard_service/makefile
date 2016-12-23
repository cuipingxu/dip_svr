
CC = g++ -DLINUX -D_REENTRANT
FLAGS = -Wall -g -fPIC

BASE_ROOT=../../../../../../sdk/base/
JSON_ROOT=../../../../../../3rd_party/sdk/json-c/
TINY_XML_ROOT=../../../../../../3rd_party/sdk/tinyxml/
CURL_ROOT=../../../../../../3rd_party/sdk/curl/
OPENSSL_ROOT=../../../../../../3rd_party/sdk/openssl/
LIBCOMM_ROOT=../../sdk/libcomm/

INCLUDES=-I./ -I$(BASE_ROOT)/include/ -I$(JSON_ROOT)/include/ \
-I$(TINY_XML_ROOT)/include/ -I$(CURL_ROOT)/include/ -I$(OPENSSL_ROOT)/include/ \
-I$(LIBCOMM_ROOT)/include/ 

LIB_PATH=

SHARE_LIBS=$(LIBCOMM_ROOT)/lib/libcomm.a $(JSON_ROOT)/lib/libjson-c.a \
$(BASE_ROOT)/lib/libbase.a $(TINY_XML_ROOT)/lib/libtinyxml.a $(CURL_ROOT)/lib/libcurl.a \
$(OPENSSL_ROOT)/lib/libssl.a $(OPENSSL_ROOT)/lib/libcrypto.a \
-lpthread -ldl -lrt -lz

TARGET=libstandard_service.so

ALL: $(TARGET)

OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

obj/%.o: %.h
obj/%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@ 

$(TARGET): $(OBJS)
	$(CC) -shared -fPIC -Wl,-soname,$(SONAME) -o $(TARGET) $(OBJS) $(SHARE_LIBS)
	cp $(TARGET) ../../sdk/libservice/
	cp $(TARGET) ../../../../release/libservice/

init:
	mkdir -p obj
	mkdir -p ../../sdk/libservice/
	
clean:
	-rm obj/*.o
	-rm ./$(TARGET)


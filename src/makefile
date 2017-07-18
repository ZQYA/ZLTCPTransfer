.PHONY : C3P0 R2D2 clean
mp_objects = $(patsubst %.cpp,%.o,$(wildcard ../../MMTP/*.cpp)) 
mp_sources = $(wildcard ../../MMTP/*.cpp)
mp_lua_source = $(wildcard lua_src/src/*.c)
mp_lua_objects = $(patsubst %.c,%.o,$(mp_lua_source))
VPATH = /Users/quanzhou/oscode/MMTP:lua_src/src 
C3P0 : C3P0.o dktool.o darkside.o dklog.o $(mp_objects) $(mp_lua_objects)
	gcc -o C3P0 C3P0.o dktool.o darkside.o dklog.o $(mp_objects) $(mp_lua_objects) -ggdb -I ../../MMTP -l stdc++
C3P0.o : C3P0.cpp $(mp_lua_objects) 
	gcc -c $(mp_lua_objects) C3P0.cpp darkside.cpp dktool.cpp dklog.cpp -ggdb -I ../../MMTP -l stdc++
dktool.o : dktool.cpp
	gcc  -c dktool.cpp  -ggdb
darkside.o : darkside.cpp 
	gcc  -c darkside.cpp -I ../../MMTP -ggdb -l stdc++
dklog.o : dklog.cpp
	gcc -c dklog.cpp -ggdb -l stdc++
$(mp_lua_objects) : $(mp_lua_source)
	$(foreach f,$(mp_lua_source),gcc -DLUA_USE_POSIX -DLUA_USE_DLOPEN -c $(f) -o $(patsubst %c,%o,$(f));)
$(mp_objects) : $(mp_sources)
	gcc -o $@ -c $^  -I ./ -ggdb
R2D2 : R2D2.o dktool.o brightside.o icmp_protocol.o dklog.o $(mp_objects)
	gcc -o R2D2 R2D2.o dktool.o icmp_protocol.o brightside.o dklog.o $(mp_objects) -ggdb -I ../../MMTP
R2D2.o : R2D2.cpp brightside.cpp icmp_protocol.cpp dklog.cpp
	gcc -c R2D2.cpp brightside.cpp icmp_protocol.cpp dklog.cpp -ggdb -I ../../MMTP -ggdb
ping: ping.o dktool.o brightside.o  $(mp_objects)
	gcc -o ping ping.o dktool.o brightside.o icmp_protocol.o $(mp_objects) -ggdb -I ../../MMTP
ping.o : ping.cpp brightside.cpp icmp_protocol.cpp 
	gcc -c ping.cpp brightside.cpp  icmp_protocol.cpp -ggdb -I ../../MMTP -ggdb
clean: 
	-rm *.o
	-rm R2D2 C3P0 ping

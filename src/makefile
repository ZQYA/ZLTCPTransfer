.PHONY : C3P0 R2D2 clean
mp_objects = $(patsubst %.cpp,%.o,$(wildcard ../../MMTP/*.cpp)) 
mp_sources = $(wildcard ../../MMTP/*.cpp)
VPATH = /Users/quanzhou/oscode/MMTP 
C3P0 : C3P0.o dktool.o darkside.o $(mp_objects)
	gcc -o C3P0 C3P0.o dktool.o darkside.o $(mp_objects) -ggdb -I ../../MMTP
C3P0.o : C3P0.cpp
	gcc -c C3P0.cpp darkside.cpp dktool.cpp -ggdb -I ../../MMTP
dktool.o : dktool.cpp
	gcc  -c dktool.cpp  -ggdb
darkside.o : darkside.cpp
	gcc  -c darkside.cpp -I ../../MMTP -ggdb 
$(mp_objects) : $(mp_sources)
	gcc -o $@ -c $^  -I ./ -ggdb

R2D2 : R2D2.o dktool.o $(mp_objects)
	gcc -o R2D2 R2D2.o dktool.o $(mp_objects) -ggdb -I ../../MMTP
R2D2.o : R2D2.cpp
	gcc -c R2D2.cpp -ggdb -I ../../MMTP -ggdb
clean: 
	-rm *.o
	-rm R2D2 C3P0

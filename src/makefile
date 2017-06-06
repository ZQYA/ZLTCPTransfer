# C3PO.cpp	R2D2.cpp	darkside.cpp	darkside.hpp	dktool.cpp	dktool.hpp	
.PHONY : C3P0 R2D2 clean
C3P0 : C3P0.o
	gcc -o C3P0 C3P0.o dktool.o darkside.o -ggdb
C3P0.o :
	gcc -c C3P0.cpp darkside.cpp dktool.cpp -ggdb

R2D2 : R2D2.o
	gcc -o R2D2 R2D2.o dktool.o -ggdb
R2D2.o :
	gcc -c R2D2.cpp dktool.cpp -ggdb

clean: 
	-rm *.o
	-rm R2D2 C3P0

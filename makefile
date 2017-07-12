main: main.cpp OneCut.h myutil.h graph.o ibfs.o maxflow.o EasyBMP.o
	g++ -g -o2 main.cpp -o main graph.o ibfs.o maxflow.o EasyBMP.o -I./ -I./EasyBMP/ -I./ibfs/ -I./maxflow/
graph.o:
	g++ -O2 -c maxflow/graph.cpp
ibfs.o:
	g++ -O2 -c ibfs/ibfs.cpp
maxflow.o:
	g++ -O2 -c maxflow/maxflow.cpp
EasyBMP.o:
	g++ -O2 -c EasyBMP/EasyBMP.cpp
clean:
	rm *.o main

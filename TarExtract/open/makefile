simple.exe:	test.cpp test.h cleanup.cpp
	g++ -g -o simple.exe test.cpp cleanup.cpp 
	echo "done compiling"

print:
	enscript -2rG test.cpp test.h cleanup.cpp
	echo "Printed source...."

clean:
	rm -f simple simple.exe


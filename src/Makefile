%.o: %.c ../lib
	gcc -C -o $@ $< -I

myclient: clicheck.c
	gcc -Wall clicheck.c -o myclient
	mv myclient ../bin
	
myserver: servcheck.c
	gcc -Wall servcheck.c -o myserver
	mv myserver ../bin
	
clean:
	rm -f ../bin/myclient ../bin/myserver ../bin/*.o
	
test: test.py
	python3 test.py

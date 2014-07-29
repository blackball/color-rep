test:
	clang -g -Wall -O3 color-rep.c test.c -o $@ $^ -lopencv_core -lopencv_highgui -lopencv_imgproc -lm

clean:
	rm test

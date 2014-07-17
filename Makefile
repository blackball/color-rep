test:
	clang color-rep.c test.c -o test -lopencv_core -lopencv_highgui -lopencv_imgproc -lm

clean:
	rm test

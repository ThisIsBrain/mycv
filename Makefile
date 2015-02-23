CC              := g++
CFLAGS          := -I/usr/local/include/opencv -L/usr/local/lib
OBJECTS         := 
LIBRARIES       := -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_gpu
NAME			:= vision

.PHONY: all clean
	
all: main

main: math.o main.o hough_transform.o contour.o  find_figure.o  
	$(CC) $(CFLAGS) -o $(NAME)  main.o contour.o hough_transform.o find_figure.o math.o $(LIBRARIES)

main.o: main.cpp mycv.h
	$(CC) $(CFLAGS) -c main.cpp $(LIBRARIES)

hough_transform.o: hough_transform.cpp hough_transform.h mycv.h
	$(CC) $(CFLAGS) -c hough_transform.cpp $(LIBRARIES)

find_figure.o: find_figure.cpp find_figure.h
	$(CC) $(CFLAGS) -c find_figure.cpp $(LIBRARIES)

contour.o: contour.cpp contour.h mycv.h 
	$(CC) $(CFLAGS) -c contour.cpp $(LIBRARIES)

math.o: math.cpp math.h mycv.h
	$(CC) $(CFLAGS) -c math.cpp $(LIBRARIES)

mycv.h: array1d.h array2d.h array3d.h array4d.h line.h math.h type.h

        
clean:
	rm -f *.o

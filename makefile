CC = c++  
LD = c++ 
CPPFLAGS = 
CFLAGS = -c 
LDFLAGS = 
LDLIBS = 
MAINFILE = test.cpp 
EXECUTABLE = myThreadSafetyQueue 

$(EXECUTABLE): main.o 
	$(LD) $(LDFLAGS) main.o $(LDLIBS) -o$(EXECUTABLE)

main.o: $(MAINFILE) 
	$(CC) $(CFLAGS) $(MAINFILE) $(CPPFLAGS) -o main.o

clean:
	rm $(EXECUTABLE) *.o



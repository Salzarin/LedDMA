CXX = gcc
CXXFLAGS =-lpthread -Wall
ODIR=obj

_OBJ = LedDMA.o gpio.o pwm.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: add_dir | leddma

add_dir:
	mkdir -p $(ODIR)
	
$(ODIR)/%.o: %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $< 
	
	
leddma: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^  

clean:	
	rm -f leddma *o *~ obj/*.o
	rmdir obj

CXX = gcc
CXXFLAGS =-g -lpthread -Wall
ODIR=obj

_OBJ = LedDMA.o gpio.o pwm.o dma.o color.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: add_dir | leddma

add_dir:
	mkdir -p $(ODIR); git pull
	
$(ODIR)/%.o: %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $< 
	
	
leddma: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^  

clean:	
	rm -f leddma *o *~ obj/*.o
	rmdir obj

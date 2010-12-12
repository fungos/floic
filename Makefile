VERSION = $$Id: $$
PROJECT=floic

include Objects.in

TARGET = bin/$(PROJECT)

# Flags/Compilers
GCC = g++
CFLAGS = -Wall -fPIC -g $(CDEFS)
CDEFS = -D LINUX -D DEBUG 
LFLAGS = -lircclient -lpthread
CPPFLAGS = -Weffc++ $(CFLAGS)
LIBS =

# targets...

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	$(GCC) -o $(TARGET) $(OBJECTS) $(LFLAGS) $(LIBS)


valgrind:
	valgrind --leak-check=full $(TARGET)

run:
	$(TARGET)

test:
	cd tests ; ./run.sh ; cd ..
#
# Cleanup
#
clean:
	rm -f $(OBJECTS) $(TARGET) *~ src/*~ core 

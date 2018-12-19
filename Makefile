.DEFAULT_GOAL := all

# location of the Python header files
PYTHON_VERSION = 3.6
PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)

# location of the Boost Python include files and library
BOOST_INC = /home/nick/boost_1_69_0
BOOST_LIB = /home/nick/boost_libs/lib

TARGET = Tetris

CFLAGS = --std=c++11

$(TARGET).so: $(TARGET).o
	g++ -shared -Wl,--export-dynamic $(TARGET).o -L$(BOOST_LIB) -lboost_python$(subst .,,$(PYTHON_VERSION)) /usr/lib/python$(PYTHON_VERSION)/config-3.6m-x86_64-linux-gnu/libpython3.6.so -o $(TARGET).so $(CFLAGS)

$(TARGET).o: $(TARGET).cpp
	g++ -I$(PYTHON_INCLUDE) -I$(BOOST_INC) -fPIC -c $(TARGET).cpp $(CFLAGS)

all: $(TARGET).so

clean:
	rm *.o *.so
.DEFAULT_GOAL := all

# location of the Python header files
PYTHON_VERSION = 3.6
# PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)
PYTHON_INCLUDE = /home/nick/anaconda3/envs/tensorflow/include/python$(PYTHON_VERSION)

# location of the Boost Python include files and library
BOOST_INC = /home/nick/boost_1_69_0
# BOOST_LIB = /home/nick/boost_libs/lib
BOOST_LIB = /home/nick/boost_libs_conda/lib

# TARGET = Tetris
OBJECTS = Tetris Tree

CFLAGS = --std=c++11 -ggdb3

# $(TARGET).so: $(TARGET).o
# 	g++ -shared -Wl,--export-dynamic $(TARGET).o -L$(BOOST_LIB) -lboost_python$(subst .,,$(PYTHON_VERSION)) -L$(BOOST_LIB) -lboost_numpy$(subst .,,$(PYTHON_VERSION)) /usr/lib/python$(PYTHON_VERSION)/config-3.6m-x86_64-linux-gnu/libpython3.6.so -o $(TARGET).so $(CFLAGS)
#
# $(TARGET).o: $(TARGET).cpp
# 	g++ -I$(PYTHON_INCLUDE) -I$(BOOST_INC) -fPIC -c $(TARGET).cpp $(CFLAGS)

$(addsuffix .so,$(OBJECTS)): %.so: %.o
	g++ -shared -Wl,--export-dynamic $< -L$(BOOST_LIB) -lboost_python$(subst .,,$(PYTHON_VERSION)) -L$(BOOST_LIB) -lboost_numpy$(subst .,,$(PYTHON_VERSION)) /usr/lib/python$(PYTHON_VERSION)/config-3.6m-x86_64-linux-gnu/libpython3.6.so -o $@ $(CFLAGS)

$(addsuffix .o,$(OBJECTS)): %.o: %.cpp
	g++ -I$(PYTHON_INCLUDE) -I$(BOOST_INC) -fPIC -c $< $(CFLAGS)


# all: $(TARGET).so
all: $(addsuffix .so,$(OBJECTS))

clean:
	rm *.o *.so

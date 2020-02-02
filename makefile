DLL = libket.so 

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -fPIC -shared -std=c++17 -g
CXXLINK = -lboost_program_options -lpthread

all: $(DLL)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(DLL): $(OBJ) 
	$(CXX) $(CXXFLAGS) $(CXXLINK) -o $@ $^

install: $(DLL)
	cp $(DLL) /usr/lib/
	cp include/ket.hpp /usr/include/ket

uninstall:
	rm /usr/lib/$(DLL)
	rm /usr/include/ket

clean:
	rm -f $(OBJ) $(DLL)

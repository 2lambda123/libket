DLL = libket.so 
CDLL = libcket.so 

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -fPIC -shared -std=c++17 -g -Wall -Werror
CXXLINK = -lboost_program_options -lpthread -lboost_serialization

all: $(DLL)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(DLL): $(OBJ) 
	$(CXX) $(CXXFLAGS) $(CXXLINK) -o $@ $^

$(CDLL): $(OBJ) c/cket.cpp
	$(CXX) $(CXXFLAGS) $(CXXLINK) -D__LIBCKET -o $@ $(OBJ) c/cket.cpp 

install: $(DLL) $(CDLL)
	cp $(DLL) /usr/lib/
	cp $(CDLL) /usr/lib/
	cp include/ket.hpp /usr/include/ket
	cp include/libket.hpp /usr/include/libket
	cp c/cket.h /usr/include/

uninstall:
	rm -f /usr/lib/$(DLL)
	rm -f /usr/lib/$(CDLL)
	rm -f /usr/include/ket
	rm -f /usr/include/libket 
	rm -f /usr/include/cket.h

clean:
	rm -f $(OBJ) $(DLL) $(CDLL)

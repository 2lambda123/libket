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
	cp $(DLL) ~/ld

clean:
	rm -f $(OBJ) $(DLL)

DLL = libket.so 

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -fPIC -shared
CXXLINK = -lboost_program_options -lpthread

all: $(DLL)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(DLL): $(OBJ) 
	$(CXX) $(CXXFLAGS) $(CXXLINK) -o $@ $^

intasll: $(DLL)
	cp $(DLL) /usr/lib/

clean:
	rm -f $(OBJ) $(DLL)

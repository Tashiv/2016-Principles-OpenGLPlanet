### Configurables ###

CXX=g++
CXXFLAGS= -c -std=c++11

INCLUDES= -Iinclude
LFLAGS= -lSDL2 -lGLEW -lGL -L/include

BUILDDIR=build
SRCDIR=src

SRC=$(wildcard $(SRCDIR)/*.cpp)
_OBJ=$(SRC:.cpp=.o)
OBJ=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(_OBJ))
TARGET=AdvGL
TARGETPATH=$(BUILDDIR)/$(TARGET)

### Default Rule ###

build: $(OBJ) $(TARGET)
	chmod 755 $(BUILDDIR)/$(TARGET)

### Special Rules ###

run: build
	cd $(BUILDDIR); ./$(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGETPATH) $(LFLAGS)


$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -o $@

clean:
	rm -f build/AdvGL build/AdvGL.exe
	rm -f build/*.o build/*.obj


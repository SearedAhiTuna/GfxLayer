
TARGET = gltest

CC	   = gcc
CXX    = g++

CFLAGS = -g -Wall -Wextra -std=c++11
LFLAGS = -lGL -lglut -lGLEW -lglfw3 -lpthread -lX11 -ldl

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin

SRC := $(wildcard $(SRCDIR)/*.cpp)
INC := $(wildcard $(INCDIR)/*.hpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

INCFLAGS := -iquote $(INCDIR)

MSBUILD = /c/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/MSBuild/Current/Bin/MSBuild.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $(CFLAGS) $(INCFLAGS) -o $(BINDIR)/$@ $^ $(LFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/*.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCFLAGS) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: pre-install
pre-install: glfw-install glew-install glm-install glut-install

external/glew-1.13.0:
	wget https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.tgz
	mkdir -p external/glew-1.13.0
	mv glew*.tgz external/
	tar -zxvf external/glew*.tgz -C external/glew-1.13.0 --strip-components 1
	rm external/glew*.tgz

.PHONY: glew-install
glew-install: external/glew-1.13.0
	cd external/glew-1.13.0/build/cmake; cmake .
	cd external/glew-1.13.0/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Debug
	cd external/glew-1.13.0/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Release
	cp -r external/glew-1.13.0/build/cmake/bin/* external/glew-1.13.0/bin

.PHONY: glfw-install
glfw-install:
	git clone https://github.com/glfw/glfw.git external/glfw || true
	cd external/glfw; cmake .
	cd external/glfw; make all
	cd external/glfw; sudo make install

.PHONY: glm-install
glm-install:
	git clone https://github.com/g-truc/glm.git external/glm || true
	cd external/glm; cmake .
	cd external/glm; make all
	cd external/glm; sudo make install

.PHONY: glut-install
glut-install:
	wget https://sourceforge.net/projects/freeglut/files/latest
	mkdir -p external/glut
	mv latest external/glut.tgz
	tar -zxvf external/glut.tgz -C external/glut --strip-components 1
	rm external/glut.tgz
	cd external/glut; cmake .
	cd external/glut; make all
	cd external/glut; sudo make install

.PHONY: clean-install
clean-install:
	rm -r external

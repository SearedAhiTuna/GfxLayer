
MSBUILD = /c/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/MSBuild/Current/Bin/MSBuild.exe

all: glfw glew glm

external/glew-1.13.0:
	wget https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.tgz
	mkdir -p external/glew-1.13.0
	mv glew*.tgz external/
	tar -zxvf external/glew*.tgz -C external/glew-1.13.0 --strip-components 1
	rm external/glew*.tgz

.PHONY: glew
glew: external/glew-1.13.0
	cd external/glew-1.13.0/build/cmake; cmake .
	cd external/glew-1.13.0/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Debug
	cd external/glew-1.13.0/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Release
	cp -r external/glew-1.13.0/build/cmake/bin/* external/glew-1.13.0/bin

.PHONY: glfw
glfw:
	cd external/glfw-3.3; cmake .
	cd external/glfw-3.3; $(MSBUILD) GLFW.sln /property:Configuration=Debug
	cd external/glfw-3.3; $(MSBUILD) GLFW.sln /property:Configuration=Release

.PHONY: glm
glm:
	cd external/glm-0.9.7; cmake .
	cd external/glm-0.9.7; $(MSBUILD) glm.sln /property:Configuration=Debug
	cd external/glm-0.9.7; $(MSBUILD) glm.sln /property:Configuration=Release

.PHONY: clean
clean:
	rm -r external

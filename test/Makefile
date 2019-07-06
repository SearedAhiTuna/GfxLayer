
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
	cd external/glew-1.13.0/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Debug /property:Platform=x64
	cd external/glew-1.13.0/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Release /property:Platform=x64
	cp -r external/glew-1.13.0/build/cmake/bin/* external/glew-1.13.0/bin
	cp -r external/glew-1.13.0/build/cmake/lib/* external/glew-1.13.0/lib

.PHONY: glfw
glfw:
	cd external/glfw-3.3; cmake .
	cd external/glfw-3.3; $(MSBUILD) GLFW.sln /property:Configuration=Debug /property:Platform=x64
	cd external/glfw-3.3; $(MSBUILD) GLFW.sln /property:Configuration=Release /property:Platform=x64

.PHONY: glm
glm:
	cd external/glm-0.9.7; cmake .
	cd external/glm-0.9.7; $(MSBUILD) glm.sln /property:Configuration=Debug /property:Platform=x64
	cd external/glm-0.9.7; $(MSBUILD) glm.sln /property:Configuration=Release /property:Platform=x64

.PHONY: freetype
freetype: external/freetype-2.10.1
	cd external/freetype-2.10.1/builds/windows/vc2010; $(MSBUILD) freetype.sln /property:Configuration=Debug /property:Platform=x64
	cd external/freetype-2.10.1/builds/windows/vc2010; $(MSBUILD) freetype.sln /property:Configuration=Release /property:Platform=x64

external/freetype-2.10.1:
	wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
	mkdir -p external/freetype-2.10.1
	mv freetype*.tar.gz external/
	tar -zxvf external/freetype*.tar.gz -C external/freetype-2.10.1 --strip-components 1
	rm external/freetype*.tar.gz


.PHONY: clean
clean:
	rm -r external

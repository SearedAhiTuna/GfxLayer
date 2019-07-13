
MSBUILD = /c/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/MSBuild/Current/Bin/MSBuild.exe

all: glfw glew glm freetype

.PHONY: glew
glew: external/glew/bin/Debug/glew32d.dll external/glew/bin/Release/glew32.dll

external/glew/bin/Debug/glew32d.dll: external/glew/build/cmake/glew.sln
	cd external/glew/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Debug /property:Platform=x64
	cp -r external/glew/build/cmake/bin/Debug/* external/glew/bin/Debug
	cp -r external/glew/build/cmake/lib/Debug/* external/glew/lib/Debug

external/glew/bin/Release/glew32.dll: external/glew/build/cmake/glew.sln
	cd external/glew/build/cmake; $(MSBUILD) glew.sln /property:Configuration=Release /property:Platform=x64
	cp -r external/glew/build/cmake/bin/Release/* external/glew/bin/Release
	cp -r external/glew/build/cmake/lib/Release/* external/glew/lib/Release

external/glew/build/cmake/glew.sln: external/glew
	cd external/glew/build/cmake; cmake .

external/glew:
	wget https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.tgz
	mkdir -p external/glew
	mv glew*.tgz external/
	tar -zxvf external/glew*.tgz -C external/glew --strip-components 1
	rm external/glew*.tgz

.PHONY: glfw
glfw: external/glfw/src/Debug/glfw3.lib external/glfw/src/Release/glfw3.lib

external/glfw/src/Debug/glfw3.lib: external/glfw/GLFW.sln
	cd external/glfw; $(MSBUILD) GLFW.sln /property:Configuration=Debug /property:Platform=x64

external/glfw/src/Release/glfw3.lib: external/glfw/GLFW.sln
	cd external/glfw; $(MSBUILD) GLFW.sln /property:Configuration=Release /property:Platform=x64

external/glfw/GLFW.sln:
	cd external/glfw; cmake .

.PHONY: glm
glm: external/glm/x64

external/glm/x64: external/glm/glm.sln
	cd external/glm; $(MSBUILD) glm.sln /property:Configuration=Debug /property:Platform=x64
	cd external/glm; $(MSBUILD) glm.sln /property:Configuration=Release /property:Platform=x64

external/glm/glm.sln:
	cd external/glm; cmake .

.PHONY: freetype
freetype: external/freetype/objs/x64/Debug/freetype.dll external/freetype/objs/x64/Release/freetype.dll

external/freetype/objs/x64/Debug/freetype.dll: external/freetype
	cd external/freetype/builds/windows/vc2010; $(MSBUILD) freetype.sln /property:Configuration=Debug /property:Platform=x64

external/freetype/objs/x64/Release/freetype.dll: external/freetype
	cd external/freetype/builds/windows/vc2010; $(MSBUILD) freetype.sln /property:Configuration=Release /property:Platform=x64

external/freetype:
	wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
	mkdir -p external/freetype
	mv freetype*.tar.gz external/
	tar -zxvf external/freetype*.tar.gz -C external/freetype --strip-components 1
	rm external/freetype*.tar.gz

.PHONY: clean
clean:
	rm -r external

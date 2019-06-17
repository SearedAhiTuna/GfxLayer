
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

external/soil:
	wget http://www.lonesock.net/files/soil.zip
	mv soil.zip external/
	unzip external/soil.zip -d external
	rm external/soil.zip
	mv external/"Simple OpenGL Image Library" external/soil

.PHONY: soil
soil: external/soil
	cd external/soil/projects/VC9; $(MSBUILD) SOIL.sln /property:Configuration=Debug /property:Platform=x64
	cd external/soil/projects/VC9; $(MSBUILD) SOIL.sln /property:Configuration=Release /property:Platform=x64
	cp external/soil/projects/VC9/x64/Debug/SOIL.lib external/soil/lib/SOIL_d.lib
	cp external/soil/projects/VC9/x64/Release/SOIL.lib external/soil/lib/SOIL.lib

external/devil:
	wget https://sourceforge.net/projects/openil/files/DevIL%20Windows%20SDK/1.8.0/DevIL-Windows-SDK-1.8.0.zip
	mv DevIL*.zip external/
	unzip external/DevIL*.zip -d external
	rm external/DevIL*.zip
	mv external/"DevIL Windows SDK" external/devil

.PHONY: devil
devil: external/devil

external/libpng-1.6.37:
	wget https://sourceforge.net/projects/libpng/files/libpng16/1.6.37/libpng-1.6.37.tar.gz
	mkdir -p external/libpng-1.6.37
	mv libpng*.tar.gz external/
	tar -zxvf external/libpng*.tar.gz -C external/libpng-1.6.37 --strip-components 1
	rm external/libpng*.tar.gz

.PHONY: libpng
libpng: external/libpng-1.6.37

.PHONY: clean
clean:
	rm -r external

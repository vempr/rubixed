OLD_OS := $(OS)
ifeq ($(OS),WINDOWS_NT)
	LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
	TARGET = rubixed.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LIBS = -lraylib -lGL -lm -lpthread -lX11
		TARGET = rubixed
	endif
	ifeq ($(UNAME_S),Darwin)
		LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
		TARGET = rubixed
	endif
endif

all:
	gcc main.c cube.c -o $(TARGET) $(LIBS)
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
	gcc -std=c99 -O3 -D_POSIX_C_SOURCE=200809L \
	-I./ckociemba/include \
	main.c \
	cube.c \
	draw.c \
	scramble.c \
	ckociemba/coordcube.c \
	ckociemba/cubiecube.c \
	ckociemba/facecube.c \
	ckociemba/search.c \
	ckociemba/prunetable_helpers.c \
	-o $(TARGET) $(LIBS)
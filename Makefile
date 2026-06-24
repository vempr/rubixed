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
	cube/cube.c \
	cube/draw.c \
	cube/anim.c \
	scramble/scramble.c \
	scramble/scramble_engine.c \
	app/app.c \
	app/theme.c \
	app/app_modes.c \
	app/app_ui.c \
	ui/ui.c \
	camera/camera.c \
	timer/timer.c \
	solver/solver.c \
	storage/log.c \
	app/utils/utils.c \
	app/utils/table.c \
	app/utils/projection.c \
	ckociemba/coordcube.c \
	ckociemba/cubiecube.c \
	ckociemba/facecube.c \
	ckociemba/search.c \
	ckociemba/prunetable_helpers.c \
	-o $(TARGET) $(LIBS)
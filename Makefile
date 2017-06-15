IR_INC		=	./src
DIR_SRC		=	./src
DIR_OBJ		=	./obj
DIR_BIN		=	./bin
EXCLUDE		=	ui_builder/

CFLAGS		=	-g -Wall -I./API/NAS2D/include `sdl-config --cflags`
LDFLAGS		=	-lstdc++ -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lphysfs -lGLU -lGL -lGLEW -L./API/NAS2D/lib/ -lnas2d

SRC			=	$(filter-out ./src/ui_builder/%.cpp,$(shell find ${DIR_SRC} -name "*.cpp"))
OBJ			=	$(subst src/,obj/,$(SRC:.cpp=.o))
INC_H		:=	$(shell find $(DIR_INC) -name '*.h')

TARGET		=	${DIR_BIN}/OPHD

all: ${TARGET}

${TARGET}: ${OBJ} ./API/NAS2D/lib/libnas2d.a
	$(CXX) $^ ${LDFLAGS} ${INCLUDES} -o $@

${DIR_OBJ}/%.o: ${DIR_SRC}/%.cpp ${INC_H}# ${INC_CPP}
	@mkdir -p ${@D}
	@dirs=$(dir $<); \
	if [ "$${dirs#*ui_builder/}" == "$$dirs" ]; \
		then ${CXX} -c $< ${CFLAGS} -o $@; \
	fi

.PHONY:clean
clean:
	-rm -fr ${DIR_OBJ}/*
	-rm -fr ${DIR_BIN}/*

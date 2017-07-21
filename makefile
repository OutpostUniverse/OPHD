# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

SRC = ./src

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CFLAGS = -std=c++11 -g -Wall -I./API/NAS2D/include `sdl-config --cflags`
LDFLAGS = -lstdc++ -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lphysfs -lGLU -lGL -lGLEW -L./API/NAS2D/lib/ -lnas2d

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS = $(shell find $(SRC) -name '*.cpp')
OBJ = $(filter-out $(SRC)/ui_builder/%,$(patsubst %.cpp,%.o,$(SRCS)))
EXE = OPHD

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

%.o : %.cpp
$(OBJ): %.o : %.cpp $(DEPDIR)/%.d
	@mkdir -p $(?D) > /dev/null
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))

.PHONY:clean
clean:
	-rm -fr $(OBJ)
	-rm -fr $(EXE)

clean-deps:
	-rm -fr $(DEPDIR)

# vim: filetype=make

# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

SRCDIR := src/
BUILDDIR := .build/
OBJDIR := $(BUILDDIR)obj/
EXE := OPHD
NAS2DDIR := nas2d-core/
NAS2DINCLUDEDIR := $(NAS2DDIR)include/
NAS2DLIBDIR := $(NAS2DDIR)lib/
NAS2DLIB := $(NAS2DLIBDIR)libnas2d.a

CPPFLAGS := $(CPPFLAGS_EXTRA)
CXXFLAGS_WARN := -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wnull-dereference -Wold-style-cast -Wcast-qual -Wcast-align -Wdouble-promotion -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-include-dirs -Winvalid-pch -Wmissing-format-attribute $(WARN_EXTRA)
CXXFLAGS := $(CXXFLAGS_EXTRA) -std=c++17 $(CXXFLAGS_WARN) -I$(NAS2DINCLUDEDIR) $(shell sdl2-config --cflags)
LDFLAGS := $(LDFLAGS_EXTRA) -L$(NAS2DLIBDIR) $(shell sdl2-config --libs)
LDLIBS := $(LDLIBS_EXTRA) -lnas2d -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lphysfs -lGL -lGLEW

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJDIR)$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(OBJDIR)$*.Td $(OBJDIR)$*.d && touch $@

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SRCS))
FOLDERS := $(sort $(dir $(SRCS)))

.PHONY: all
all: $(EXE)

$(EXE): $(NAS2DLIB) $(OBJS)
	@mkdir -p ${@D}
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $@

.PHONY: intermediate
intermediate: $(OBJS)

$(NAS2DLIB): nas2d

.PHONY: nas2d
nas2d:
	$(MAKE) -C nas2d-core

$(OBJS): $(OBJDIR)%.o : $(SRCDIR)%.cpp $(OBJDIR)%.d | build-folder
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

.PHONY: build-folder
build-folder:
	@mkdir -p $(patsubst $(SRCDIR)%,$(OBJDIR)%, $(FOLDERS))

$(OBJDIR)%.d: ;
.PRECIOUS: $(OBJDIR)%.d

include $(wildcard $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.d,$(SRCS)))

.PHONY: clean clean-all
clean:
	-rm -fr $(OBJDIR)
clean-all:
	-rm -rf $(BUILDDIR)
	-rm -f $(EXE)


.PHONY: lint
lint: cppcheck cppclean

.PHONY: cppcheck
cppcheck:
	cppcheck --quiet "$(SRCDIR)"

.PHONY: cppclean
cppclean:
	cppclean --quiet --include-path "$(NAS2DINCLUDEDIR)" --include-path "/usr/include/SDL2" --exclude "MicroPather" "$(SRCDIR)"

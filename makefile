# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

SRCDIR := src
BUILDDIR := .build
BINDIR := .
OBJDIR := $(BUILDDIR)/obj
DEPDIR := $(BUILDDIR)/deps
EXE := $(BINDIR)/OPHD
NAS2DDIR := nas2d-core
NAS2DINCLUDEDIR := $(NAS2DDIR)/include
NAS2DLIBDIR := $(NAS2DDIR)/lib
NAS2DLIB := $(NAS2DLIBDIR)/libnas2d.a

CFLAGS := -std=c++11 -g -Wall -Wno-unknown-pragmas -I$(NAS2DINCLUDEDIR) $(shell sdl2-config --cflags)
LDFLAGS := -lstdc++ -lm -L$(NAS2DLIBDIR) -lnas2d \
	$(shell sdl2-config --libs) -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf \
	-lphysfs -lGLU -lGL -lGLEW

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
FOLDERS := $(sort $(dir $(SRCS)))

.PHONY: all
all: $(EXE)

$(EXE): $(NAS2DLIB) $(OBJS)
	@mkdir -p ${@D}
	$(CXX) $^ $(LDFLAGS) -o $@

$(NAS2DLIB): nas2d

.PHONY: nas2d
nas2d:
	$(MAKE) -C nas2d-core

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d | build-folder
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

.PHONY: build-folder
build-folder:
	@mkdir -p $(patsubst $(SRCDIR)/%,$(OBJDIR)/%, $(FOLDERS))
	@mkdir -p $(patsubst $(SRCDIR)/%,$(DEPDIR)/%, $(FOLDERS))

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(SRCS)))

.PHONY: clean clean-deps clean-all
clean:
	-rm -fr $(OBJDIR)
	-rm -fr $(DEPDIR)
clean-deps:
	-rm -fr $(DEPDIR)
clean-all:
	-rm -rf $(BUILDDIR)

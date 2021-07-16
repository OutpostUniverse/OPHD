# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

SRCDIR := OPHD/
BUILDDIR := .build/
OBJDIR := $(BUILDDIR)obj/
EXE := ophd.exe
NAS2DDIR := nas2d-core/
NAS2DINCLUDEDIR := $(NAS2DDIR)
NAS2DLIBDIR := $(NAS2DDIR)lib/
NAS2DLIB := $(NAS2DLIBDIR)libnas2d.a
PACKAGEDIR := $(BUILDDIR)/package/

# Determine OS (Linux, Darwin, ...)
CURRENT_OS := $(shell uname 2>/dev/null || echo Unknown)
TARGET_OS ?= $(CURRENT_OS)

Linux_OpenGL_LIBS := -lGLEW -lGL
Darwin_OpenGL_LIBS := -lGLEW -framework OpenGL
Windows_OpenGL_LIBS := -lglew32 -lopengl32
OpenGL_LIBS := $($(TARGET_OS)_OpenGL_LIBS)

CPPFLAGS := $(CPPFLAGS_EXTRA)
CXXFLAGS_WARN := -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wnull-dereference -Wold-style-cast -Wcast-qual -Wcast-align -Wdouble-promotion -Wfloat-conversion -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-include-dirs -Winvalid-pch -Wmissing-format-attribute $(WARN_EXTRA)
CXXFLAGS := $(CXXFLAGS_EXTRA) -std=c++17 $(CXXFLAGS_WARN) -I$(NAS2DINCLUDEDIR) $(shell sdl2-config --cflags)
LDFLAGS := $(LDFLAGS_EXTRA) -L$(NAS2DLIBDIR) $(shell sdl2-config --libs)
LDLIBS := $(LDLIBS_EXTRA) -lnas2d -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lphysfs $(OpenGL_LIBS)

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
nas2d: $(NAS2DDIR)makefile
	$(MAKE) -C nas2d-core

$(NAS2DDIR)makefile:
	@echo "\nWARNING: NAS2D dependency not found. Install as Git submodule or download manually."
	@if test -d ".git"; then \
		echo "Git detected. Attempting to download submodules.\n"; \
		git submodule update --init --recursive; \
	else \
		echo "You don't seem to be using Git. Consider using Git clone.\n"; \
		false; \
	fi

$(OBJS): $(OBJDIR)%.o : $(SRCDIR)%.cpp $(OBJDIR)%.d | build-folder
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

.PHONY: build-folder
build-folder:
	@mkdir -p $(patsubst $(SRCDIR)%,$(OBJDIR)%, $(FOLDERS))

$(OBJDIR)%.d: ;
.PRECIOUS: $(OBJDIR)%.d

include $(wildcard $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.d,$(SRCS)))


VERSION = $(shell git describe --tags --dirty)
CONFIG = $(TARGET_OS).x64
PACKAGE_NAME = $(PACKAGEDIR)ophd-$(VERSION)-$(CONFIG).tar.gz

.PHONY: package
package: $(PACKAGE_NAME)

$(PACKAGE_NAME): $(EXE)
	@mkdir -p "$(PACKAGEDIR)"
	tar -czf $(PACKAGE_NAME) $(EXE)


.PHONY: clean clean-all
clean:
	-rm -fr $(OBJDIR)
clean-all:
	-rm -rf $(BUILDDIR)
	-rm -f $(EXE)


.PHONY: install-dependencies
install-dependencies:
	$(MAKE) -C nas2d-core install-dependencies


.PHONY: lint
lint: cppcheck cppclean

.PHONY: cppcheck
cppcheck:
	cppcheck --quiet "$(SRCDIR)"

.PHONY: cppclean
cppclean:
	cppclean --quiet --include-path "$(NAS2DINCLUDEDIR)" --include-path "/usr/include/SDL2" --exclude "MicroPather" "$(SRCDIR)"

.PHONY: format
format:
	clang-format --version
	find OPHD/ \( -name '*.cpp' -o -name '*.h' \) \! -name 'resource.h' -o -path 'OPHD/MicroPather' -prune -type f | xargs clang-format -i

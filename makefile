# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

CONFIG := Debug
Debug_CXX_FLAGS := -Og -g
Release_CXX_FLAGS := -O3
CONFIG_CXX_FLAGS := $($(CONFIG)_CXX_FLAGS)

# Determine OS (Linux, Darwin, ...)
CURRENT_OS := $(shell uname 2>/dev/null || echo Unknown)
TARGET_OS ?= $(CURRENT_OS)

ROOTBUILDDIR := .build
BUILDDIRPREFIX := $(ROOTBUILDDIR)/$(CONFIG)_Linux_


## Default and top-level targets ##

.PHONY: all
all: ophd


## NAS2D project ##

NAS2DDIR := nas2d-core/
NAS2DINCLUDEDIR := $(NAS2DDIR)
NAS2DLIBDIR := $(NAS2DDIR)lib/
NAS2DLIB := $(NAS2DLIBDIR)libnas2d.a

$(NAS2DLIB): nas2d ;

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


## Default project flags ##

Linux_OpenGL_LIBS := -lGLEW -lGL
FreeBSD_OpenGL_LIBS := $(Linux_OpenGL_LIBS)
Darwin_OpenGL_LIBS := -lGLEW -framework OpenGL
Windows_OpenGL_LIBS := -lglew32 -lopengl32
OpenGL_LIBS := $($(TARGET_OS)_OpenGL_LIBS)

CPPFLAGS := $(CPPFLAGS_EXTRA)
CXXFLAGS_WARN := -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wnull-dereference -Wold-style-cast -Wcast-qual -Wcast-align -Wdouble-promotion -Wfloat-conversion -Wsign-conversion -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-include-dirs -Winvalid-pch -Wmissing-format-attribute $(WARN_EXTRA)
CXXFLAGS := $(CXXFLAGS_EXTRA) $(CONFIG_CXX_FLAGS) -std=c++20 $(CXXFLAGS_WARN) -I$(NAS2DINCLUDEDIR) $(shell sdl2-config --cflags)
LDFLAGS := $(LDFLAGS_EXTRA) -L$(NAS2DLIBDIR) $(shell sdl2-config --libs)
LDLIBS := $(LDLIBS_EXTRA) -lnas2d -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf $(OpenGL_LIBS)

PROJECT_FLAGS := $(CPPFLAGS) $(CXXFLAGS)
PROJECT_LINKFLAGS := $(LDFLAGS) $(LDLIBS)


## OPHD project ##

ophd_SRCDIR := OPHD/
ophd_OBJDIR := $(BUILDDIRPREFIX)$(ophd_SRCDIR)Intermediate/
ophd_OUTPUT := ophd.exe
ophd_SRCS := $(shell find $(ophd_SRCDIR) -name '*.cpp')
ophd_OBJS := $(patsubst $(ophd_SRCDIR)%.cpp,$(ophd_OBJDIR)%.o,$(ophd_SRCS))

.PHONY: ophd
ophd: $(ophd_OUTPUT)

$(ophd_OUTPUT): $(ophd_OBJS) $(NAS2DLIB)
$(ophd_OBJS): $(ophd_OBJDIR)%.o : $(ophd_SRCDIR)%.cpp $(ophd_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(ophd_OBJS)))


.PHONY: intermediate
intermediate: $(ophd_OBJS)


## Compile rules ##

DEPFLAGS = -MT $@ -MMD -MP -MF $(@:.o=.Td)
COMPILE.cpp = $(CXX) $(DEPFLAGS) $(PROJECT_FLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(@:.o=.Td) $(@:.o=.d) && touch $@


%:
	@mkdir -p "${@D}"
	$(CXX) $^ $(PROJECT_LINKFLAGS) -o $@

lib%.a:
	@mkdir -p "${@D}"
	ar rcs $@ $^

%.o:
	@mkdir -p "${@D}"
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.d: ;
.PRECIOUS: %.d


## Clean ##

.PHONY: clean clean-all
clean:
	-rm -fr $(ophd_OBJDIR)
clean-all:
	-rm -rf $(ROOTBUILDDIR)
	-rm -f $(ophd_OUTPUT)


## Package ##

PACKAGEDIR := $(ROOTBUILDDIR)/package/
VERSION = $(shell git describe --tags --dirty)
CONFIG = $(TARGET_OS).x64
PACKAGE_NAME = $(PACKAGEDIR)ophd-$(VERSION)-$(CONFIG).tar.gz

.PHONY: package
package: $(PACKAGE_NAME)

$(PACKAGE_NAME): $(ophd_OUTPUT)
	@mkdir -p "$(PACKAGEDIR)"
	tar -czf $(PACKAGE_NAME) $(ophd_OUTPUT)


## Dependencies ##

.PHONY: install-dependencies
install-dependencies:
	$(MAKE) -C nas2d-core install-dependencies


## Linting ##

.PHONY: show-warnings
show-warnings:
	$(MAKE) -j1 clean all CXX=clang++ CXXFLAGS_WARN=-Weverything 2>&1 >/dev/null | grep -o "\[-W.*\]" | sort | uniq

.PHONY: lint
lint: cppcheck cppclean cppinclude

.PHONY: cppcheck
cppcheck:
	cppcheck --quiet "$(ophd_SRCDIR)"

.PHONY: cppclean
cppclean:
	cppclean --quiet --include-path "$(NAS2DINCLUDEDIR)" --include-path "/usr/include/SDL2" --exclude "MicroPather" "$(ophd_SRCDIR)"

.PHONY: cppinclude
cppinclude:
	cppinclude

.PHONY: format
format:
	@clang-format --version
	find $(ophd_SRCDIR) \( -name '*.cpp' -o -name '*.h' \) \! -name 'resource.h' -o -path '$(ophd_SRCDIR)MicroPather' -prune -type f | xargs clang-format -i

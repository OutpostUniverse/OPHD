# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

CONFIG := Debug
Debug_CXX_FLAGS := -Og -g
Release_CXX_FLAGS := -O3
CONFIG_CXX_FLAGS := $($(CONFIG)_CXX_FLAGS)

# Determine OS (Linux, Darwin, ...)
CURRENT_OS := $(shell uname 2>/dev/null || echo Unknown)
TARGET_OS ?= $(CURRENT_OS)

Windows_RUN_PREFIX := wine
RUN_PREFIX := $($(TARGET_OS)_RUN_PREFIX)

Windows_EXE_SUFFIX := .exe
EXE_SUFFIX := $($(TARGET_OS)_EXE_SUFFIX)

ROOTBUILDDIR := .build
BUILDDIRPREFIX := $(ROOTBUILDDIR)/$(CONFIG)_Linux_


## Default and top-level targets ##

.PHONY: all
all: ophd$(EXE_SUFFIX)

.PHONY: test
test: testLibOPHD testLibControls

.PHONY: check
check: checkOPHD checkControls


## NAS2D project ##

NAS2DDIR := nas2d-core/
NAS2DINCLUDEDIR := $(NAS2DDIR)
NAS2DLIB := $(NAS2DDIR)lib/libnas2d.a

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

SDL_CONFIG := sdl2-config
SDL_CONFIG_CFLAGS = $(shell $(SDL_CONFIG) --cflags)
SDL_CONFIG_LIBS = $(shell $(SDL_CONFIG) --libs)

CPPFLAGS := $(CPPFLAGS_EXTRA)
CXXFLAGS_WARN := -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wnull-dereference -Wold-style-cast -Wcast-qual -Wcast-align -Wdouble-promotion -Wfloat-conversion -Wsign-conversion -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-include-dirs -Winvalid-pch -Wmissing-format-attribute $(WARN_EXTRA)
CXXFLAGS := $(CXXFLAGS_EXTRA) $(CONFIG_CXX_FLAGS) -std=c++20 $(CXXFLAGS_WARN) -I$(NAS2DINCLUDEDIR) $(SDL_CONFIG_CFLAGS)
LDFLAGS := $(LDFLAGS_EXTRA) $(SDL_CONFIG_LIBS)
LDLIBS := $(LDLIBS_EXTRA) -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2 $(OpenGL_LIBS)

PROJECT_FLAGS := $(CPPFLAGS) $(CXXFLAGS)
PROJECT_LINKFLAGS := $(LDFLAGS) $(LDLIBS)


## libOPHD project ##

libOPHD_SRCDIR := libOPHD/
libOPHD_OUTDIR := $(BUILDDIRPREFIX)$(libOPHD_SRCDIR)
libOPHD_OBJDIR := $(libOPHD_OUTDIR)Intermediate/
libOPHD_OUTPUT := $(libOPHD_OUTDIR)libOPHD.a
libOPHD_SRCS := $(shell find $(libOPHD_SRCDIR) -name '*.cpp')
libOPHD_OBJS := $(patsubst $(libOPHD_SRCDIR)%.cpp,$(libOPHD_OBJDIR)%.o,$(libOPHD_SRCS))

.PHONY: libOphd
libOphd: $(libOPHD_OUTPUT)

$(libOPHD_OUTPUT): $(libOPHD_OBJS)
$(libOPHD_OBJS): $(libOPHD_OBJDIR)%.o : $(libOPHD_SRCDIR)%.cpp $(libOPHD_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(libOPHD_OBJS)))


## libControls project ##

libControls_SRCDIR := libControls/
libControls_OUTDIR := $(BUILDDIRPREFIX)$(libControls_SRCDIR)
libControls_OBJDIR := $(libControls_OUTDIR)Intermediate/
libControls_OUTPUT := $(libControls_OUTDIR)libControls.a
libControls_SRCS := $(shell find $(libControls_SRCDIR) -name '*.cpp')
libControls_OBJS := $(patsubst $(libControls_SRCDIR)%.cpp,$(libControls_OBJDIR)%.o,$(libControls_SRCS))

.PHONY: libControls
libControls: $(libControls_OUTPUT)

$(libControls_OUTPUT): $(libControls_OBJS)
$(libControls_OBJS): $(libControls_OBJDIR)%.o : $(libControls_SRCDIR)%.cpp $(libControls_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(libControls_OBJS)))


## testLibOPHD project ##

testLibOphd_SRCDIR := testLibOPHD/
testLibOphd_OBJDIR := $(BUILDDIRPREFIX)$(testLibOphd_SRCDIR)Intermediate/
testLibOphd_OUTPUT := $(BUILDDIRPREFIX)$(testLibOphd_SRCDIR)testLibOPHD$(EXE_SUFFIX)
testLibOphd_SRCS := $(shell find $(testLibOphd_SRCDIR) -name '*.cpp')
testLibOphd_OBJS := $(patsubst $(testLibOphd_SRCDIR)%.cpp,$(testLibOphd_OBJDIR)%.o,$(testLibOphd_SRCS))

testLibOphd_CPPFLAGS := $(CPPFLAGS) -I./
testLibOphd_LDLIBS := -lgmock_main -lgmock -lgtest -lpthread $(LDLIBS_EXTRA)

testLibOphd_PROJECT_FLAGS := $(testLibOphd_CPPFLAGS) $(CXXFLAGS)
testLibOphd_PROJECT_LINKFLAGS = $(LDFLAGS_EXTRA) $(testLibOphd_LDLIBS)

.PHONY: testLibOPHD
testLibOPHD: $(testLibOphd_OUTPUT)

.PHONY: checkOPHD
checkOPHD: $(testLibOphd_OUTPUT)
	$(RUN_PREFIX) $(testLibOphd_OUTPUT)

$(testLibOphd_OUTPUT): PROJECT_LINKFLAGS := $(testLibOphd_PROJECT_LINKFLAGS)
$(testLibOphd_OUTPUT): $(testLibOphd_OBJS) $(libOPHD_OUTPUT) $(NAS2DLIB)

$(testLibOphd_OBJS): PROJECT_FLAGS := $(testLibOphd_PROJECT_FLAGS)
$(testLibOphd_OBJS): $(testLibOphd_OBJDIR)%.o : $(testLibOphd_SRCDIR)%.cpp $(testLibOphd_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(testLibOphd_OBJS)))


## testLibControls project ##

testLibControls_SRCDIR := testLibControls/
testLibControls_OBJDIR := $(BUILDDIRPREFIX)$(testLibControls_SRCDIR)Intermediate/
testLibControls_OUTPUT := $(BUILDDIRPREFIX)$(testLibControls_SRCDIR)testLibControls$(EXE_SUFFIX)
testLibControls_SRCS := $(shell find $(testLibControls_SRCDIR) -name '*.cpp')
testLibControls_OBJS := $(patsubst $(testLibControls_SRCDIR)%.cpp,$(testLibControls_OBJDIR)%.o,$(testLibControls_SRCS))

testLibControls_CPPFLAGS := $(CPPFLAGS) -I./
testLibControls_LDLIBS := -lgmock_main -lgmock -lgtest -lpthread $(LDLIBS)

testLibControls_PROJECT_FLAGS := $(testLibControls_CPPFLAGS) $(CXXFLAGS)
testLibControls_PROJECT_LINKFLAGS = $(LDFLAGS_EXTRA) $(testLibControls_LDLIBS)

.PHONY: testLibControls
testLibControls: $(testLibControls_OUTPUT)

.PHONY: checkControls
checkControls: $(testLibControls_OUTPUT)
	$(RUN_PREFIX) $(testLibControls_OUTPUT)

$(testLibControls_OUTPUT): PROJECT_LINKFLAGS := $(testLibControls_PROJECT_LINKFLAGS)
$(testLibControls_OUTPUT): $(testLibControls_OBJS) $(libControls_OUTPUT) $(NAS2DLIB)

$(testLibControls_OBJS): PROJECT_FLAGS := $(testLibControls_PROJECT_FLAGS)
$(testLibControls_OBJS): $(testLibControls_OBJDIR)%.o : $(testLibControls_SRCDIR)%.cpp $(testLibControls_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(testLibControls_OBJS)))


## demoLibControls project ##

demoLibControls_SRCDIR := demoLibControls/
demoLibControls_OBJDIR := $(BUILDDIRPREFIX)$(demoLibControls_SRCDIR)Intermediate/
demoLibControls_OUTPUT := $(BUILDDIRPREFIX)$(demoLibControls_SRCDIR)demoLibControls$(EXE_SUFFIX)
demoLibControls_SRCS := $(shell find $(demoLibControls_SRCDIR) -name '*.cpp')
demoLibControls_OBJS := $(patsubst $(demoLibControls_SRCDIR)%.cpp,$(demoLibControls_OBJDIR)%.o,$(demoLibControls_SRCS))

demoLibControls_CPPFLAGS := $(CPPFLAGS) -I./
demoLibControls_PROJECT_FLAGS := $(demoLibControls_CPPFLAGS) $(CXXFLAGS)

.PHONY: demoLibControls
demoLibControls: $(demoLibControls_OUTPUT)

.PHONY: runDemoControls
runDemoControls: $(demoLibControls_OUTPUT)
	$(RUN_PREFIX) $(demoLibControls_OUTPUT)

$(demoLibControls_OUTPUT): $(demoLibControls_OBJS) $(libControls_OUTPUT) $(NAS2DLIB)

$(demoLibControls_OBJS): PROJECT_FLAGS := $(demoLibControls_PROJECT_FLAGS)
$(demoLibControls_OBJS): $(demoLibControls_OBJDIR)%.o : $(demoLibControls_SRCDIR)%.cpp $(demoLibControls_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(demoLibControls_OBJS)))


## OPHD project ##

ophd_SRCDIR := appOPHD/
ophd_OBJDIR := $(BUILDDIRPREFIX)$(ophd_SRCDIR)Intermediate/
ophd_OUTPUT := ophd$(EXE_SUFFIX)
ophd_SRCS := $(shell find $(ophd_SRCDIR) -name '*.cpp')
ophd_OBJS := $(patsubst $(ophd_SRCDIR)%.cpp,$(ophd_OBJDIR)%.o,$(ophd_SRCS))

ophd_CPPFLAGS := $(CPPFLAGS) -I./
ophd_PROJECT_FLAGS := $(ophd_CPPFLAGS) $(CXXFLAGS)

$(ophd_OUTPUT): $(ophd_OBJS) $(libOPHD_OUTPUT) $(libControls_OUTPUT) $(NAS2DLIB)

$(ophd_OBJS): PROJECT_FLAGS := $(ophd_PROJECT_FLAGS)
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
	-rm -fr $(libOPHD_OBJDIR)
	-rm -fr $(libControls_OBJDIR)
	-rm -fr $(testLibOphd_OBJDIR)
	-rm -fr $(testLibControls_OBJDIR)
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
	@$(MAKE) clean > /dev/null
	$(MAKE) --output-sync all CXX=clang++ CXXFLAGS_WARN=-Weverything 2>&1 >/dev/null | grep -o "\[-W.*\]" | sort | uniq
	@$(MAKE) clean > /dev/null

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

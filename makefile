# Source http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

## Default and top-level targets ##

.DEFAULT_GOAL := ophd

.PHONY: all
all: ophd test demoLibControls

.PHONY: test
test: testLibOPHD testLibControls

.PHONY: check
check: all checkOPHD checkControls


# Determine OS (Linux, Darwin, ...)
CURRENT_OS := $(shell uname 2>/dev/null || echo Unknown)
TARGET_OS ?= $(CURRENT_OS)

# Toolchain: gcc, clang, mingw, (or blank for environment default)
Toolchain ?=

PkgConfig := pkg-config
WarnFlags := -Wall -Wextra -Wpedantic -Wzero-as-null-pointer-constant -Wnull-dereference -Wold-style-cast -Wcast-qual -Wcast-align -Wdouble-promotion -Wfloat-conversion -Wsign-conversion -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-declarations -Wmissing-include-dirs -Winvalid-pch -Wunknown-pragmas -Wmissing-format-attribute -Wredundant-decls -Wformat=2

gccCXX := g++
gccWarnFlags := $(WarnFlags) -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast -Weffc++
gccPkgConfig := $(PkgConfig)
gccTARGET_OS := $(TARGET_OS)

clangCXX := clang++
clangWarnNotInterested := -Wno-c++98-compat-pedantic -Wno-pre-c++17-compat
clangWarnAllowed := -Wno-padded -Wno-cast-function-type-strict
clangWarnKnown := -Wno-unsafe-buffer-usage -Wno-global-constructors -Wno-exit-time-destructors -Wno-unused-member-function
clangWarnShow := -Weverything $(clangWarnNotInterested)
clangWarnFlags := $(clangWarnShow) $(clangWarnAllowed) $(clangWarnKnown)
clangPkgConfig := $(PkgConfig)
clangTARGET_OS := $(TARGET_OS)

mingwCXX := x86_64-w64-mingw32-g++
mingwWarnFlags := $(WarnFlags)
mingwPkgConfig := x86_64-w64-mingw32-pkg-config
mingwTARGET_OS := Windows

CXX := $($(Toolchain)CXX)
WarnFlags := $($(Toolchain)WarnFlags)
PkgConfig := $($(Toolchain)PkgConfig)
TARGET_OS := $($(Toolchain)TARGET_OS)

# Build configuration
CONFIG := Debug
Debug_CXX_FLAGS := -Og -g
Release_CXX_FLAGS := -O3
CONFIG_CXX_FLAGS := $($(CONFIG)_CXX_FLAGS)

# Target specific settings
WindowsSpecialPreprocessorFlags = -DGLEW_STATIC -DSDL_MAIN_HANDLED
WindowsSpecialWarnFlags = -Wno-redundant-decls
WindowsExeSuffix := .exe
WindowsRunPrefix := wine
WindowsRunSuffixUnitTest := --gtest_color=yes | cat -

DarwinIncludeSearchFlags = -isystem$(shell brew --prefix)/include

IncludeSearchFlags := $($(TARGET_OS)IncludeSearchFlags)
SpecialPreprocessorFlags := $($(TARGET_OS)SpecialPreprocessorFlags)
SpecialWarnFlags := $($(TARGET_OS)SpecialWarnFlags)
ExeSuffix := $($(TARGET_OS)ExeSuffix)
RunPrefix := $($(TARGET_OS)RunPrefix)
RunSuffixUnitTest := $($(TARGET_OS)RunSuffixUnitTest)

ROOTBUILDDIR := .build
BUILDDIRPREFIX := $(ROOTBUILDDIR)/$(CONFIG)_Linux_


## NAS2D project ##

NAS2DDIR := nas2d-core/
NAS2DINCLUDEDIR := $(NAS2DDIR)
NAS2DLIB := $(NAS2DDIR)lib/libnas2d.a

$(NAS2DLIB): nas2d ;

.PHONY: nas2d
nas2d: $(NAS2DDIR)makefile
	@$(MAKE) --no-print-directory --directory nas2d-core

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

IncludeSearchFlags := $(shell type $(PkgConfig) >/dev/null 2>&1 && $(PkgConfig) --cflags-only-I sdl2) $(IncludeSearchFlags)
LibrarySearchFlags := $(shell type $(PkgConfig) >/dev/null 2>&1 && $(PkgConfig) --libs-only-L sdl2)

Linux_OpenGL_LIBS := -lGLEW -lGL
FreeBSD_OpenGL_LIBS := $(Linux_OpenGL_LIBS)
Darwin_OpenGL_LIBS := -lGLEW -framework OpenGL
Windows_OpenGL_LIBS := -lglew32 -lopengl32
OpenGL_LIBS := $($(TARGET_OS)_OpenGL_LIBS)

CPPFLAGS := -I$(NAS2DINCLUDEDIR) $(IncludeSearchFlags) $(SpecialPreprocessorFlags) $(CPPFLAGS_EXTRA)
CXXFLAGS_WARN := $(WarnFlags) $(SpecialWarnFlags) $(WARN_EXTRA)
CXXFLAGS := $(CXXFLAGS_EXTRA) $(CONFIG_CXX_FLAGS) -std=c++20 $(CXXFLAGS_WARN)
LDFLAGS := $(LibrarySearchFlags) $(LDFLAGS_EXTRA)
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
testLibOphd_OUTPUT := $(BUILDDIRPREFIX)$(testLibOphd_SRCDIR)testLibOPHD$(ExeSuffix)
testLibOphd_SRCS := $(shell find $(testLibOphd_SRCDIR) -name '*.cpp')
testLibOphd_OBJS := $(patsubst $(testLibOphd_SRCDIR)%.cpp,$(testLibOphd_OBJDIR)%.o,$(testLibOphd_SRCS))

testLibOphd_CPPFLAGS := -I./ $(CPPFLAGS)
testLibOphd_LDLIBS := -lgmock_main -lgmock -lgtest -lpthread $(LDLIBS_EXTRA)

testLibOphd_PROJECT_FLAGS := $(testLibOphd_CPPFLAGS) $(CXXFLAGS)
testLibOphd_PROJECT_LINKFLAGS = $(LDFLAGS) $(testLibOphd_LDLIBS)

.PHONY: testLibOPHD
testLibOPHD: $(testLibOphd_OUTPUT)

.PHONY: checkOPHD
checkOPHD: $(testLibOphd_OUTPUT)
	$(RunPrefix) $(testLibOphd_OUTPUT) $(GTEST_OPTIONS) $(RunSuffixUnitTest)

$(testLibOphd_OUTPUT): PROJECT_LINKFLAGS := $(testLibOphd_PROJECT_LINKFLAGS)
$(testLibOphd_OUTPUT): $(testLibOphd_OBJS) $(libOPHD_OUTPUT) $(NAS2DLIB)

$(testLibOphd_OBJS): PROJECT_FLAGS := $(testLibOphd_PROJECT_FLAGS)
$(testLibOphd_OBJS): $(testLibOphd_OBJDIR)%.o : $(testLibOphd_SRCDIR)%.cpp $(testLibOphd_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(testLibOphd_OBJS)))


## testLibControls project ##

testLibControls_SRCDIR := testLibControls/
testLibControls_OBJDIR := $(BUILDDIRPREFIX)$(testLibControls_SRCDIR)Intermediate/
testLibControls_OUTPUT := $(BUILDDIRPREFIX)$(testLibControls_SRCDIR)testLibControls$(ExeSuffix)
testLibControls_SRCS := $(shell find $(testLibControls_SRCDIR) -name '*.cpp')
testLibControls_OBJS := $(patsubst $(testLibControls_SRCDIR)%.cpp,$(testLibControls_OBJDIR)%.o,$(testLibControls_SRCS))

testLibControls_CPPFLAGS := -I./ $(CPPFLAGS)
testLibControls_LDLIBS := -lgmock_main -lgmock -lgtest -lpthread $(LDLIBS)

testLibControls_PROJECT_FLAGS := $(testLibControls_CPPFLAGS) $(CXXFLAGS)
testLibControls_PROJECT_LINKFLAGS = $(LDFLAGS) $(testLibControls_LDLIBS)

.PHONY: testLibControls
testLibControls: $(testLibControls_OUTPUT)

.PHONY: checkControls
checkControls: $(testLibControls_OUTPUT)
	$(RunPrefix) $(testLibControls_OUTPUT) $(GTEST_OPTIONS) $(RunSuffixUnitTest)

$(testLibControls_OUTPUT): PROJECT_LINKFLAGS := $(testLibControls_PROJECT_LINKFLAGS)
$(testLibControls_OUTPUT): $(testLibControls_OBJS) $(libControls_OUTPUT) $(NAS2DLIB)

$(testLibControls_OBJS): PROJECT_FLAGS := $(testLibControls_PROJECT_FLAGS)
$(testLibControls_OBJS): $(testLibControls_OBJDIR)%.o : $(testLibControls_SRCDIR)%.cpp $(testLibControls_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(testLibControls_OBJS)))


## demoLibControls project ##

demoLibControls_SRCDIR := demoLibControls/
demoLibControls_OBJDIR := $(BUILDDIRPREFIX)$(demoLibControls_SRCDIR)Intermediate/
demoLibControls_OUTPUT := $(BUILDDIRPREFIX)$(demoLibControls_SRCDIR)demoLibControls$(ExeSuffix)
demoLibControls_SRCS := $(shell find $(demoLibControls_SRCDIR) -name '*.cpp')
demoLibControls_OBJS := $(patsubst $(demoLibControls_SRCDIR)%.cpp,$(demoLibControls_OBJDIR)%.o,$(demoLibControls_SRCS))

demoLibControls_CPPFLAGS := -I./ $(CPPFLAGS)
demoLibControls_PROJECT_FLAGS := $(demoLibControls_CPPFLAGS) $(CXXFLAGS)

.PHONY: demoLibControls
demoLibControls: $(demoLibControls_OUTPUT)

.PHONY: runDemoControls
runDemoControls: $(demoLibControls_OUTPUT)
	$(RunPrefix) $(demoLibControls_OUTPUT)

$(demoLibControls_OUTPUT): $(demoLibControls_OBJS) $(libControls_OUTPUT) $(NAS2DLIB)

$(demoLibControls_OBJS): PROJECT_FLAGS := $(demoLibControls_PROJECT_FLAGS)
$(demoLibControls_OBJS): $(demoLibControls_OBJDIR)%.o : $(demoLibControls_SRCDIR)%.cpp $(demoLibControls_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(demoLibControls_OBJS)))


## OPHD project ##

ophd_SRCDIR := appOPHD/
ophd_OBJDIR := $(BUILDDIRPREFIX)$(ophd_SRCDIR)Intermediate/
ophd_OUTPUT := $(BUILDDIRPREFIX)$(ophd_SRCDIR)ophd$(ExeSuffix)
ophd_SRCS := $(shell find $(ophd_SRCDIR) -name '*.cpp')
ophd_OBJS := $(patsubst $(ophd_SRCDIR)%.cpp,$(ophd_OBJDIR)%.o,$(ophd_SRCS))

ophd_CPPFLAGS := -I./ $(CPPFLAGS)
ophd_PROJECT_FLAGS := $(ophd_CPPFLAGS) $(CXXFLAGS)

$(ophd_OUTPUT): $(ophd_OBJS) $(libOPHD_OUTPUT) $(libControls_OUTPUT) $(NAS2DLIB)

$(ophd_OBJS): PROJECT_FLAGS := $(ophd_PROJECT_FLAGS)
$(ophd_OBJS): $(ophd_OBJDIR)%.o : $(ophd_SRCDIR)%.cpp $(ophd_OBJDIR)%.d

include $(wildcard $(patsubst %.o,%.d,$(ophd_OBJS)))


.PHONY: intermediate
intermediate: $(ophd_OBJS)

.PHONY: ophd
ophd: $(ophd_OUTPUT)

.PHONY: run
run: $(ophd_OUTPUT)
	$(RunPrefix) $(ophd_OUTPUT) $(OPHD_RUN_FLAGS)


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
	-rm -fr $(demoLibControls_OBJDIR)
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
	$(MAKE) --no-print-directory --directory nas2d-core install-dependencies


## Linting ##

.PHONY: show-warnings
show-warnings:
	@$(MAKE) clean > /dev/null
	$(MAKE) --output-sync all CXX=clang++ CXXFLAGS_WARN="$(clangWarnShow)" 2>&1 >/dev/null | grep -o "\[-W.*\]" | sort | uniq
	@$(MAKE) clean > /dev/null

.PHONY: lint
lint: cppcheck cppclean cppinclude

.PHONY: cppcheck
cppcheck:
	cppcheck --quiet "$(ophd_SRCDIR)"

.PHONY: cppclean
cppclean:
	cppclean --quiet --include-path "$(NAS2DINCLUDEDIR)" --include-path "/usr/include/SDL2" --exclude "MicroPather" --exclude "Structures.h" --exclude "Robots.h" "$(ophd_SRCDIR)"

.PHONY: cppinclude
cppinclude:
	cppinclude

.PHONY: format
format:
	@clang-format --version
	find $(ophd_SRCDIR) \( -name '*.cpp' -o -name '*.h' \) \! -name 'resource.h' -o -path '$(ophd_SRCDIR)MicroPather' -prune -type f | xargs clang-format -i


## Debugging ##

.PHONY: stale
stale:
	@$(MAKE) -n | grep -oE '[^ ]+\.cpp$$' || true

.PHONY: stale-objs
stale-objs:
	@$(MAKE) -n | grep -oE '[^ ]+\.o$$' || true

# This can create a lot of extra tab auto complete entries, so maybe disable by default
AllObjectFiles := $(ophd_OBJS)) $(libOPHD_OBJS) $(libControls_OBJS) $(demoLibControls_OBJS) $(testLibOPHD_OBJS) $(testLibControls_OBJS)
AllObjectShortNames := #$(sort $(basename $(notdir $(AllObjectFiles))))
.PHONY: $(AllObjectShortNames)
$(AllObjectShortNames):
	$(MAKE) $(filter %$@.o,$(AllObjectFiles))


## Compile performance ##

.PHONY: flame-charts
flame-charts:
	$(MAKE) all CXX=clang++ CXXFLAGS_EXTRA="-ftime-trace"


## GitHub ##
.PHONY: cache-list-all cache-list-main cache-list-branch cache-delete-main-stale cache-delete-branch
GhCacheKeyIncremental := ophd-
GhCacheLimit := 100
GhCacheFields := id,ref,key,version,sizeInBytes,createdAt,lastAccessedAt
GhCacheListJson := gh cache list --limit $(GhCacheLimit) --json $(GhCacheFields)
GhCacheListMain := $(GhCacheListJson) --ref refs/heads/main --key $(GhCacheKeyIncremental) --jq 'sort_by(.lastAccessedAt)'
GhCacheListBranch := $(GhCacheListJson) --jq '.[] | select(.ref!="refs/heads/main")'
GhCacheDeleteIds := xargs -I '{}' gh cache delete '{}'

cache-list-all:
	$(GhCacheListJson)

cache-list-main:
	$(GhCacheListMain)

cache-list-branch:
	$(GhCacheListBranch)

cache-delete-main-stale:
	$(GhCacheListMain) | jq '.[0:-2] | .[] .id' | $(GhCacheDeleteIds)

cache-delete-branch:
	$(GhCacheListBranch) | jq '.id' | $(GhCacheDeleteIds)

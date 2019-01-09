CXXFLAGS += -std=c++11 -Wall -Wextra -Werror -MMD -I$(abspath src)
LDFLAGS += -lboost_program_options -lboost_filesystem -lboost_iostreams
LDFLAGS += -lboost_system

INSTALL := install -D

ifneq ($(OS),Windows_NT)
    bin_suffix :=
else
    bin_suffix := .exe
endif

# this function of two arguments (array and element) returns index of the
# element in the array; return -1 if item not found in the list
pos = $(strip $(eval T := ) \
              $(eval i := -1) \
              $(foreach elem, $1, \
                        $(if $(filter $2,$(elem)), \
                                      $(eval i := $(words $T)), \
                                      $(eval T := $T $(elem)))) \
              $i)

# determine output directory and build target; "." is the directory by default
# or "release"/"debug" for corresponding targets
is_release := 0
ifneq ($(call pos,release,$(MAKECMDGOALS)),-1)
    is_release := 1
endif
ifneq ($(call pos,install,$(MAKECMDGOALS)),-1)
    is_release := 1
endif
ifneq ($(is_release),0)
    EXTRA_CXXFLAGS := -O3
    EXTRA_LDFLAGS  := -Wl,--strip-all

    out_dir := release
    target  := release
else
    EXTRA_CXXFLAGS := -O0 -g
    EXTRA_LDFLAGS  := -g

    ifneq ($(call pos,debug,$(MAKECMDGOALS)),-1)
        out_dir := debug
    else
        with_cov := 0
        ifneq ($(call pos,coverage,$(MAKECMDGOALS)),-1)
            with_cov := 1
        endif

        ifeq ($(with_cov),1)
            out_dir := coverage
            EXTRA_CXXFLAGS += --coverage -DNDEBUG
            EXTRA_LDFLAGS  += --coverage
        else
            out_dir := .
        endif
    endif
    target := debug
endif

# traverse directories ($1) recursively looking for a pattern ($2) to make list
# of matching files
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) \
                                        $(filter $(subst *,%,$2),$d))

bin := dit$(bin_suffix)

bin_sources := $(call rwildcard, src/, *.cpp)
bin_objects := $(bin_sources:%.cpp=$(out_dir)/%.o)
bin_depends := $(bin_sources:%.cpp=$(out_dir)/%.d)

tests_sources := $(call rwildcard, tests/, *.cpp)
tests_objects := $(tests_sources:%.cpp=$(out_dir)/%.o)
tests_depends := $(tests_sources:%.cpp=$(out_dir)/%.d)

out_dirs := $(sort $(dir $(bin_objects) $(tests_objects)))

.PHONY: check clean man debug release
.PHONY: coverage reset-coverage
.PHONY: install uninstall

debug release: $(out_dir)/$(bin)

man: docs/dit.1
docs/dit.1: $(wildcard docs/*.md) | $(out_dir)/docs
	pandoc -V title=dit \
	       -V section=1 \
	       -V app=dit \
	       -V date="$$(date +'%B %d, %Y')" \
	       -V author='xaizek <xaizek@posteo.net>' \
	       -s -o $@ $(sort $^)

coverage: check
	gcov -p $(bin_objects) > /dev/null
	uncov-gcov --root . --build-root . --no-gcov --capture-worktree \
	           --exclude tests | uncov new
	find . -name '*.gcov' -delete

reset-coverage:
ifeq ($(with_cov),1)
	find $(out_dir)/ -name '*.gcda' -delete
endif

$(out_dir)/$(bin): $(bin_objects) | $(out_dirs)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

check: $(target) $(out_dir)/tests/tests reset-coverage
	@$(out_dir)/tests/tests

install: release
	$(INSTALL) $(out_dir)/$(bin) $(DESTDIR)/usr/bin/$(bin)
	$(INSTALL) -m 644 scripts/bash-completion \
	                  $(DESTDIR)/usr/share/bash-completion/completions/dit
	$(INSTALL) -m 644 docs/dit.1 $(DESTDIR)/usr/share/man/man1/dit.1

uninstall:
	$(RM) $(DESTDIR)/usr/bin/$(bin) $(DESTDIR)/usr/share/man/man1/dit.1 \
	      $(DESTDIR)/usr/share/bash-completion/completions/dit

# work around parenthesis warning in tests somehow caused by ccache
$(out_dir)/tests/tests: EXTRA_CXXFLAGS += -Wno-error=parentheses -Itests/
$(out_dir)/tests/tests: $(filter-out %/main.o,$(bin_objects)) $(tests_objects) \
                        tests/. \
                      | $(out_dirs)
	$(CXX) -o $@ $(filter-out %/main.o,$(bin_objects)) $(tests_objects) \
           $(LDFLAGS) $(EXTRA_LDFLAGS)

$(out_dir)/%.o: %.cpp | $(out_dirs)
	$(CXX) -o $@ -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $<

$(out_dirs) $(out_dir)/docs:
	mkdir -p $@

clean:
	-$(RM) -r coverage/ debug/ release/
	-$(RM) $(bin_objects) $(bin_depends) $(tests_objects) $(tests_depends) \
           $(out_dir)/$(bin) $(out_dir)/tests/tests

include $(wildcard $(bin_depends) $(tests_depends))

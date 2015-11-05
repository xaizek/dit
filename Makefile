CXXFLAGS += -std=c++11 -Wall -Wextra -Werror -MMD -I$(abspath src)
LDFLAGS += -lboost_program_options -lboost_filesystem -lboost_system

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
ifneq ($(call pos,release,$(MAKECMDGOALS)),-1)
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
        ifneq ($(call pos,show-coverage,$(MAKECMDGOALS)),-1)
            with_cov := 1
        endif

        ifneq ($(with_cov),0)
            out_dir := coverage
            EXTRA_CXXFLAGS += --coverage
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

bin := scribe$(bin_suffix)

bin_sources := $(call rwildcard, src/, *.cpp)
bin_sources := $(wildcard $(bin_sources))
bin_objects := $(bin_sources:%.cpp=$(out_dir)/%.o)
bin_depends := $(bin_sources:%.cpp=$(out_dir)/%.d)

tests_sources := $(call rwildcard, tests/, *.cpp)
tests_sources := $(wildcard $(tests_sources))
tests_objects := $(tests_sources:%.cpp=$(out_dir)/%.o)
tests_depends := $(tests_sources:%.cpp=$(out_dir)/%.d)

out_dirs := $(sort $(dir $(bin_objects) $(tests_objects)))

.PHONY: check clean coverage show-coverage reset-coverage debug release

debug release: $(out_dir)/$(bin)

coverage: check $(out_dir)/$(bin)
	lcov --directory $(out_dir)/ --base-directory src/ --capture \
	     --output-file $(out_dir)/lcov.info --config-file lcovrc \
	     --test-name unit_tests --quiet
	genhtml --output-directory $(out_dir)/data/ $(out_dir)/lcov.info \
	     --config-file lcovrc --demangle-cpp --show-details

show-coverage: coverage
	$$BROWSER coverage/data/index.html

reset-coverage:
ifneq ($(with_cov),0)
	lcov --directory $(out_dir)/ --zerocounters
endif

$(out_dir)/$(bin): $(bin_objects) | $(out_dirs)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

check: $(target) $(out_dir)/tests/tests reset-coverage
	@$(out_dir)/tests/tests

# work around parenthesis warning in tests somehow caused by ccache
$(out_dir)/tests/tests: EXTRA_CXXFLAGS += -Wno-error=parentheses -Itests/
$(out_dir)/tests/tests: $(filter-out %/main.o,$(bin_objects)) $(tests_objects) \
                        tests/. \
                      | $(out_dirs)
	$(CXX) -o $@ $(filter-out %/main.o,$(bin_objects)) $(tests_objects) \
           $(LDFLAGS) $(EXTRA_LDFLAGS)

$(out_dir)/%.o: %.cpp | $(out_dirs)
	$(CXX) -o $@ -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $<

$(out_dirs):
	mkdir -p $@

clean:
	-$(RM) -r coverage/ debug/ release/
	-$(RM) $(bin_objects) $(bin_depends) $(tests_objects) $(tests_depends) \
           $(out_dir)/$(bin) $(out_dir)/tests/tests

include $(wildcard $(bin_depends) $(tests_depends))

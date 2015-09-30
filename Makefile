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

# determine output directory, "." by default or "release"/"debug" for
# corresponding targets
ifneq ($(call pos,release,$(MAKECMDGOALS)),-1)
    out_dir := release
else
    ifneq ($(call pos,debug,$(MAKECMDGOALS)),-1)
        out_dir := debug
    else
        out_dir := .
    endif
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

release: EXTRA_CXXFLAGS := -O3
release: EXTRA_LDFLAGS := -Wl,--strip-all
debug: EXTRA_CXXFLAGS := -O0 -g
debug: EXTRA_LDFLAGS := -g
debug release: $(out_dir)/$(bin)

.PHONY: clean check

$(out_dir)/$(bin): $(bin_objects) | $(out_dirs)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

check: $(out_dir)/tests/tests
	@$(out_dir)/tests/tests

$(out_dir)/tests/tests: $(filter-out %/main.o,$(bin_objects)) $(tests_objects) \
                      | $(out_dirs)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

$(out_dir)/%.o: %.cpp | $(out_dirs)
	$(CXX) -o $@ -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $<

$(out_dirs):
	mkdir -p $@

clean:
	-$(RM) -r debug/ release/
	-$(RM) $(bin_objects) $(bin_depends) $(tests_objects) $(tests_depends) \
           $(out_dir)/$(bin) $(out_dir)/tests/tests

include $(wildcard $(bin_depends))

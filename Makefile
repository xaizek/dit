CXXFLAGS += -std=c++11 -Wall -Wextra -Werror -MMD -I$(abspath .)
LDFLAGS += -lboost_program_options -lboost_filesystem -lboost_system

ifneq ($(OS),Windows_NT)
    bin_suffix :=
else
    bin_suffix := .exe
endif

# determine output directory, "." by default or "release"/"debug" for
# corresponding targets
ifeq ($(MAKECMDGOALS),release)
    out_dir := release
else
    ifeq ($(MAKECMDGOALS),debug)
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

bin_sources := $(call rwildcard, , *.cpp)
bin_sources := $(wildcard $(bin_sources))
bin_objects := $(bin_sources:%.cpp=$(out_dir)/%.o)
bin_depends := $(bin_sources:%.cpp=$(out_dir)/%.d)
out_dirs    := $(sort $(dir $(bin_objects)))

release: EXTRA_CXXFLAGS := -O3
release: EXTRA_LDFLAGS := -Wl,--strip-all
debug: EXTRA_CXXFLAGS := -O0 -g
debug: EXTRA_LDFLAGS := -g
debug release: $(out_dir)/$(bin)

.PHONY: clean

$(out_dir)/$(bin): $(bin_objects) | $(out_dirs)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

$(out_dir)/%.o: %.cpp | $(out_dirs)
	$(CXX) -o $@ -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $<

$(out_dirs):
	mkdir -p $@

clean:
	-$(RM) -r debug/ release/
	-$(RM) $(bin_objects) $(bin_depends) $(out_dir)/$(bin)

include $(wildcard $(bin_depends))

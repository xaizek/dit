CXXFLAGS += -std=c++11 -Wall -Wextra -Werror -MMD -I$(abspath .)
LDFLAGS += -lboost_filesystem -lboost_system

ifneq ($(OS),Windows_NT)
    bin_suffix :=
else
    bin_suffix := .exe
endif

# traverse directories ($1) recursively looking for a pattern ($2) to make list
# of matching files
rwildcard = $(foreach d,$(wildcard $1/*),$(call rwildcard,$d/,$2) \
                                         $(filter $(subst *,%,$2),$d))

bin := scribe$(bin_suffix)

bin_sources := $(call rwildcard, ., *.cpp)
bin_sources := $(wildcard $(bin_sources))
bin_objects := $(bin_sources:.cpp=.o)
bin_depends := $(bin_sources:.cpp=.d)

release: EXTRA_CXXFLAGS := -O3
debug: EXTRA_CXXFLAGS := -O0 -g
debug: EXTRA_LDFLAGS := -g
debug release: $(bin)

$(bin): $(bin_objects)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $<

clean:
	-$(RM) $(bin_objects) $(bin_depends) $(bin)

include $(wildcard $(bin_depends))

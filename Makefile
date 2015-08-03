CXXFLAGS += -std=c++11 -Wall -Wextra -Werror -MMD

ifneq ($(OS),Windows_NT)
    bin_suffix :=
else
    bin_suffix := .exe
endif

bin := scribe$(bin_suffix)

bin_sources := *.cpp
bin_sources := $(wildcard $(bin_sources))
bin_objects := $(bin_sources:.cpp=.o)

$(bin): $(bin_objects)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

clean:
	-$(RM) *.o *.d $(bin)

include $(wildcard *.d)

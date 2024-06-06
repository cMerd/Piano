CXX := g++
CXXFLAGS := -Wall -lraylib -I./inc/ --std=c++20
SRCDIR := src
BUILDDIR := build

# List of source files
SRCS := $(wildcard $(SRCDIR)/*.cpp)

# Generate object file names from source file names
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Default target for Linux
$(BUILDDIR)/piano: $(OBJS)
	$(CXX) $(CXXFLAGS) -lm $^ -o $@

# Rule to compile each source file into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Move the binary to path (/usr/bin)
install:
	sudo ln -s $(shell pwd)/build/piano /bin/piano
	sudo cp ./assets/piano.desktop /usr/share/applications/piano.desktop

# Remove the binary from path (/usr/bin)
uninstall:
	sudo rm -f /bin/piano
	sudo rm -f /usr/share/applications/piano.desktop

# Clean target to remove all files in build directory
clean:
	rm -f $(BUILDDIR)/*



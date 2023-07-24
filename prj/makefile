# Makefile for the prj executable

# Target executable name
EXE = prj

# List of source files to compile
SOURCES = main.cpp imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp

# Generate object file names from the source file names
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

# Detect the current operating system
UNAME_S := $(shell uname -s)

# Libraries for Linux
LINUX_GL_LIBS = -lGL

# Compiler flags (common for all platforms)
CXXFLAGS = -std=c++11 -g -Wall -Wformat

# Libraries (platform-specific)
LIBS =

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) # LINUX
    ECHO_MESSAGE = "Linux"
    LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`
    CXXFLAGS += `pkg-config --cflags glfw3`
endif

ifeq ($(UNAME_S), Darwin) # APPLE (Mac OS X)
    ECHO_MESSAGE = "Mac OS X"
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
    LIBS += -lglfw
    CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
endif

ifeq ($(OS), Windows_NT) # Windows (MinGW)
    ECHO_MESSAGE = "MinGW"
    LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32
    CXXFLAGS += `pkg-config --cflags glfw3`
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

# Rule to compile each source file to its corresponding object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Default target: Build the executables
all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

# Linking rule to build the executable
$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) -L. -limplot

# Rule to clean generated files
clean:
	rm -f $(EXE) $(OBJS)

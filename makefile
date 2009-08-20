

# LINUX
LIBLUA=lua5.1
# MAC OSX
#LIBLUA=lua

# LDFLAGS=-arch x86_64
OBJS = swarm.o group.o scene.o particle.o vmath.o tolua_group.o tolua_swarm.o tolua_particle.o tolua_vmath.o
CXX = g++
CXXFLAGS = -Wall -c -g -O2 `sdl-config --cflags`
LDFLAGS = -Wall `sdl-config --libs`
INCLUDES = -I./include -I/usr/include/lua5.1 -I/opt/local/include
LIBS = -L./lib -L/Developer/SDKs/MacOSX10.5.sdk/usr/X11R6/lib -lANN -lGL -lGLU -llo -ltolua++5.1 -l$(LIBLUA)
TOLUA = tolua++5.1

tolua_%.cpp tolua_%.h : %.pkg
	$(TOLUA) -o $(@:%.h=%.cpp) -H $(@:%.cpp=%.h) $<

%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

# the executable
swarm: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)



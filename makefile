

LIBLUA=lua
#LIBLUA=lua5.1

LDFLAGS=-arch x86_64
OBJS = swarm.o group.o scene.o vmath.o tolua_group.o tolua_swarm.o tolua_vmath.o
CXX = g++
CXXFLAGS = -Wall -c -O2
LFLAGS = -Wall
INCLUDES = -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include
LIBS = -L./lib -L/Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/lib -L/opt/local/lib -lANN -lGL -lSDL -lGLU -llo -ltolua++5.1 -l$(LIBLUA)

# # tolua
# tolua++5.1 -o tolua_group.cpp -H tolua_group.h group.pkg
# tolua++5.1 -o tolua_swarm.cpp -H tolua_swarm.h swarm.pkg
# tolua++5.1 -o tolua_vmath.cpp -H tolua_vmath.h vmath.pkg

%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

# the executable
swarm: $(OBJS)
	$(CXX) $(LFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)



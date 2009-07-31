#!/bin/bash

# tolua
tolua++5.1 -o tolua_group.cpp -H tolua_group.h group.pkg
tolua++5.1 -o tolua_swarm.cpp -H tolua_swarm.h swarm.pkg
tolua++5.1 -o tolua_vmath.cpp -H tolua_vmath.h vmath.pkg

LIBLUA=lua
#LIBLUA=lua5.1

# the executable
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c swarm.cpp
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c group.cpp
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c scene.cpp
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c vmath.cpp
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c tolua_group.cpp
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c tolua_swarm.cpp
g++ -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -I/opt/local/include -O2 -c tolua_vmath.cpp
g++ -oswarm swarm.o group.o -L./lib -L/Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/lib -L/opt/local/lib -lANN -lGL -lSDL -lGLU -llo -ltolua++5.1 -l$LIBLUA


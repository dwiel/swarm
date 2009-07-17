#!/bin/bash

# tolua
tolua++5.1 -o tolua_group.cpp -H tolua_group.h group.pkg
tolua++5.1 -o tolua_swarm.cpp -H tolua_swarm.h swarm.pkg
tolua++5.1 -o tolua_vmath.cpp -H tolua_vmath.h vmath.pkg

# the executable
g++ swarm.cpp group.cpp scene.cpp vmath.cpp tolua_group.cpp tolua_swarm.cpp tolua_vmath.cpp -oswarm -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -L./lib -lANN -lGL -lSDL -lGLU -llo -ltolua++5.1 -llua5.1 -O2

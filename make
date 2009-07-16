#!/bin/bash

# tolua
tolua++5.1 -o tolua_group.cpp group.pkg

# the executable
g++ swarm.cpp group.cpp scene.cpp vmath.cpp tolua_group.cpp -oswarm -I/usr/include/SDL/ -I./include -I/usr/include/lua5.1 -L./lib -lANN -lGL -lSDL -lGLU -llo -ltolua++5.1 -llua5.1 -O2

#!/bin/bash

g++ swarm.cpp group.cpp scene.cpp vmath.cpp -oswarm -I/usr/include/SDL/ -I./include -L./lib -lANN -lGL -lSDL -lGLU -g

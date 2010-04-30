#ifndef SWARM_HPP
#define SWARM_HPP

extern bool keys[512];

float gettime();

void setBackgroundColor(float r, float g, float b);
void saveTGA();
void saveTGA_init();

void sendOSC(float data);

#endif
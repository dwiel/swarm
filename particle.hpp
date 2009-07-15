#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "vmath.h"

struct particle {     // Create A Structure For Particle
	bool	active;      // Active (Yes/No)
	float	fade;        // Fade Speed
	float	r;
	float	g;
	float	b;
  float a;
	Vector3f pos;
	Vector3f vel;
	int zone;          // zone for determining distance
};

#endif // PARTICLE_HPP
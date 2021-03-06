#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "vmath.h"

class particle {     // Create A Structure For Particle
public:
  particle();
  ~particle();
  
	bool	active;      // Active (Yes/No)
	float	r;
	float	g;
	float	b;
  float a;
	Vector3f pos;
	Vector3f vel;
	int zone;          // zone for determining distance
	
	float life;
};

#endif // PARTICLE_HPP
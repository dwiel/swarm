#ifndef SCENE_H
#define SCENE_H

#include "vmath.h"

class Scene {
public:
	Scene();
	
  float zoom;
	float speed;
  Vector3f pos;
};

#endif // SCENE_H
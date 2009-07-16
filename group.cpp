#include "group.hpp"

#include <GL/gl.h>					// Header File For The OpenGL 
#include <GL/glu.h>					// Header File For The GLu

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include <stdlib.h>
#include <iostream>

#define MAX_KNN_SIZE 10

using namespace std;

set<Group*> Group::groups;

bool operator<(const ppair& p1, const ppair& p2) {
	return p1.distance > p2.distance;
}

Group::Group() {
	maximum_velocity = 10.0f;
	pause_movement = false;
	speed = 2.0f;
	scene = 0;
  controlled = false;
  vel_render_scale_x = 0.01;
  vel_render_scale_y = 0.01;
  render_base_size_x = 0.5;
  render_base_size_y = 0.5;
  pos = Vector3f(0,0,0);
  move_to_neighbor_center_weight = 0.01;
  stay_in_bounds_weight = 0.0001;
  avoid_touching_weight = 0.01;
  num_particles = 500;

  boost::mt19937 rng;
  boost::normal_distribution<float> ndist(0, 5);
  boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > normr(rng, ndist);

  // initialize all particles
  particles = new particle[num_particles];
  for(int i = 0; i < num_particles; ++i) {
    particles[i].active = true;
    particles[i].a = 1.0f;
    particles[i].pos = Vector3f(normr(), normr(), normr());
    particles[i].vel = Vector3f(normr(), normr(), normr());
    push_back(&particles[i]);
  }

  nnIdx = new ANNidx[MAX_KNN_SIZE];            // allocate near neigh indices
  dists = new ANNdist[MAX_KNN_SIZE];           // allocate near neighbor dists
  
  dataPtsSize = 1;
  dataPts = annAllocPts(dataPtsSize, 3);
  color_off = 10;
  
  groups.insert(this);
}

Group::~Group() {
  delete [] nnIdx;
  delete [] dists;
  
  groups.erase(this);
}

void Group::Draw() {
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		particle* p = *iter;
		float x = p->pos.x;                         // Grab Our Particle X Position
		float y = p->pos.y;                         // Grab Our Particle Y Position
		float z = p->pos.z + this->scene->zoom;     // Particle Z Pos + Zoom
		
		glColor4f(p->r, p->g, p->b, p->a);
		
		// optionally elongate particles based on velocity
    float vx = this->render_base_size_x;
    float vy = this->render_base_size_y;
    
 		vx += p->vel.x*this->vel_render_scale_x;
 		vy += p->vel.y*this->vel_render_scale_y;
		
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(1,1); glVertex3f(x+vx, y+vy,z); // Top Right
			glTexCoord2d(0,1); glVertex3f(x-vx, y+vy,z); // Top Left
			glTexCoord2d(1,0); glVertex3f(x+vx, y-vy,z); // Bottom Right
			glTexCoord2d(0,0); glVertex3f(x-vx, y-vy,z); // Bottom Left
		glEnd();
	}
}

// probably wont be changing.
// update the position based on the scene->speed, the group->speed, each 
// particles' speed and the time elapsed since the previous movement
void Group::Move(float timediff) {
  this->buildKNN();
  
	this->figureVelocities();
	
	float gspeed;
	if(this->scene) {
		gspeed = this->speed * scene->speed * timediff;
	} else {
		gspeed = this->speed * timediff;
	}
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		if (!this->pause_movement) {
			particle* p = *iter;
			p->pos += p->vel * gspeed;
		}
	}
  
  this->destroyKNN();
}

float f(float n) {
  float x = fmod(n, 3.0f);
  if(x > 1 && x < 2) {
    return 2-x;
  } else if(x > 0 && x <= 1) {
    return x;
  } else {
    return 0;
  }
}

void Group::figureVelocities() {
	int cnt = 0;
	float len;
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		particle* p = *iter;
    getNNearestNeighbors(&neighbors, p, 5);

    p->vel += moveToNeighborsCenter(p) * this->move_to_neighbor_center_weight;
 		p->vel += stayInBounds(p) * this->stay_in_bounds_weight;
		p->vel += avoidTouching(p) * this->avoid_touching_weight;
		
		// impose maximume velocity
		len = p->vel.length();
		if(len > maximum_velocity) {
			p->vel.normalize();
			p->vel *= maximum_velocity;
		}
		
    // generate color from velocity and color offset
    p->r = p->vel.x*f(color_off)     + p->vel.y*f(color_off - 1) + p->vel.z*f(color_off - 2);
    p->g = p->vel.x*f(color_off - 1) + p->vel.y*f(color_off - 2) + p->vel.z*f(color_off);
    p->b = p->vel.x*f(color_off - 2) + p->vel.y*f(color_off)     + p->vel.z*f(color_off - 1);

    p->r = 1;
    p->g = 0;
    p->b = 0;
// 
//     if((p->pos - this->pos).length() > 10) {
//       p->g = 1;
//     }
	}
}

Vector3f Group::avoidTouching(particle* p) {
	Vector3f avoid = Vector3f(0,0,0);
	//static list<ppair> neighbors;
  //neighbors.clear();
	//getNearestNeighbors(&neighbors, p, 0.00f);
  int near = 0;
	for(vector<ppair>::iterator i = neighbors.begin(); i != neighbors.end(); ++i) {
    if(i->distance < 1.0f) {
      avoid += i->point->pos - p->pos;
      ++near;
    }
	}
	if(near) {
		return avoid / (float)near;
	} else {
		return avoid;
	}
}

// keep inside a general sphere around the point (0, 0, 0)
Vector3f Group::stayInBounds(particle* p) {
  Vector3f dist = p->pos - this->pos;
	float len = dist.length();
	if(len > 10) {
		return -dist * (len - 10);
	} else {
    return Vector3f(0,0,0);
  }
}

void Group::resizeDataPts() {
  if(this->size() > this->dataPtsSize) {
    delete [] this->dataPts;
    this->dataPtsSize = this->size();
    this->dataPts = annAllocPts(this->dataPtsSize, 3);
  }
}

void Group::buildKNN() {
  this->resizeDataPts();
  int i = 0;
  for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
    particle* p = *iter;
    this->dataPts[i] = (ANNcoord*)&p->pos;
    ++i;
  }
  
  kdTree = new ANNkd_tree(this->dataPts, this->size(), 3);
}

void Group::destroyKNN() {
  delete kdTree;
}

// uses nearest neighbors
Vector3f Group::moveToNeighborsCenter(particle* p) {
//	static vector<ppair> neighbors;
//  neighbors.clear();
//	getNNearestNeighbors(&neighbors, p, 5);
	Vector3f center;
	Vector3f veltotal;
	for(vector<ppair>::iterator i = neighbors.begin(); i != neighbors.end(); ++i) {
		center += i->point->pos;
		veltotal += i->point->vel;
	}
  
  float size = neighbors.size();
  //cout << "size" << size;
	if( size ) {
		center /= size;
		veltotal /= size;
		return (center - p->pos) + (veltotal - p->vel)*0;
	} else {
		return Vector3f(0.0f, 0.0f, 0.0f);
	}
}

float point_distance(particle* p1, particle* p2) {
	Vector3f diff = p1->pos - p2->pos;
	return sqrt(diff.dotProduct(diff));
}

void Group::getNNearestNeighbors(vector<ppair>* neighbors, particle* p1, int num) {
  if(num > MAX_KNN_SIZE) {
    printf("knn searching for too many points");
    exit(1);
  }
  
  // TODO: is this slow?
  neighbors->resize(num);
  
  kdTree->annkSearch(             // search
        (ANNcoord*)&p1->pos,      // query point
        num,                      // number of near neighbors
        this->nnIdx,              // nearest neighbors (returned)
        this->dists,              // distance (returned)
        0);                       // error bound
  
  for(int i = 0; i < num; ++i) {
    (*neighbors)[i].distance = this->dists[i];
    (*neighbors)[i].point = (*this)[this->nnIdx[i]];
  }
}

// // return a list of all neighbors to point p1 which are most max_dist away 
// // sorted by proximity 
// void Group::getNearestNeighbors(list<ppair>* neighbors, particle* p1, float max_dist) {
// 	priority_queue<ppair> distances = calcDistances(p1);
// 	
// 	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
// 		particle* p2 = *iter;
// 		if(p1 != p2) {
// 			distances.push(ppair(point_distance(p1, p2), p2));
// 		}
// 	}
// 	
// 	// priority_queue<ppair>::const_iterator iter = distances.begin();
// 	while( true ) {
// 		ppair point = distances.top();
// 		if(point.distance < max_dist) {
// 			neighbors->push_back(point);
// 			distances.pop();
// 		} else {
// 			break;
// 		}
// 	}
// 	
// //  	printf("distances: %d\n", distances.size());
// }





// Notes
// Nate will send:
//   overall amplitude
//   all sin wave frequencies
//     
//
// I will send:
//   variance
//   average position? (not really helpful with the models created right now)
//   




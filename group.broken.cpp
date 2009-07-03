#include "group.hpp"

#include <GL/gl.h>					// Header File For The OpenGL 
#include <GL/glu.h>					// Header File For The GLu

#include <stdlib.h>
#include <iostream>

#define MAX_KNN_SIZE 5

using namespace std;

bool operator<(const ppair& p1, const ppair& p2) {
	return p1.distance > p2.distance;
}

Group::Group() {
	this->maximum_velocity = 2.0f;
	this->pause_movement = false;
	this->speed = 20.0f;
	this->scene = 0;
  this->controlled = false;
  this->vel_render_scale_x = 0.0001;
  this->vel_render_scale_y = 0.0001;

  this->nnIdx = new ANNidx[MAX_KNN_SIZE];            // allocate near neigh indices
  this->dists = new ANNdist[MAX_KNN_SIZE];           // allocate near neighbor dists
  this->dataPtsSize = 1;
  this->dataPts = annAllocPts(1, 3);
}

Group::~Group() {
  delete [] this->nnIdx;
  delete [] this->dists;
  delete [] this->dataPts;
}

void Group::DrawParticles() {
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		particle* p = *iter;
		float x = p->pos.x;                         // Grab Our Particle X Position
		float y = p->pos.y;                         // Grab Our Particle Y Position
		float z = p->pos.z + this->scene->zoom;     // Particle Z Pos + Zoom
		
		glColor4f(p->r, p->g, p->b, p->life);
		
		// optionally elongate particles based on velocity
 		float vx = 0.5f + p->vel.x*this->vel_render_scale_x;
 		float vy = 0.5f + p->vel.y*this->vel_render_scale_y;
		
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(1,1); glVertex3f(x+vx, y+vy,z);	// Top Right
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

void Group::resizeDataPts() {
  if(this->size() > this->dataPtsSize) {
    delete [] this->dataPts;
    this->dataPtsSize = this->size();
    this->dataPts = annAllocPts(this->dataPtsSize, 3);
  }
}

void Group::figureVelocities() {
  this->buildKNN();
  
	int cnt = 0;
	float len;
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		particle* p = *iter;
		// TODO: this doesn't work because later algorithms pop() off the queue and
		// expect nobody to do the same.
// 		recent_distances = calcDistances(p);
		
		p->vel += moveToNeighborsCenter(p) * 0.001;
 		p->vel += stayInBounds(p) * 0.001;
		// p->vel += avoidTouching(p) * 0.01;
		
		// impose maximume velocity
		len = p->vel.length();
		if(len > maximum_velocity) {
			p->vel.normalize();
			p->vel *= maximum_velocity;
		}
		
		if(p->vel.x == 0.0f and p->vel.z == 0.0f) {
			++cnt;
		}
		
    // generate color from velocity
		p->r = fabs(p->vel.x);
		p->g = fabs(p->vel.y);
		p->b = fabs(p->vel.z);
	}
//	cout << cnt << endl;
}

Vector3f Group::avoidTouching(particle* p) {
	Vector3f avoid;
	static list<ppair> neighbors;
  neighbors.clear();
	getNearestNeighbors(&neighbors, p, 0.00f);
	for(list<ppair>::iterator i = neighbors.begin(); i != neighbors.end(); ++i) {
		avoid += i->point->pos - p->pos;
	}
	if(neighbors.size()) {
		return avoid / (float)neighbors.size();
	} else {
		return avoid;
	}
}

// keep inside a general sphere around the point (0, 0, 0)
Vector3f Group::stayInBounds(particle* p) {
	float len = p->pos.length();
	if(len > 10) {
		return -p->pos * (len - 10);
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
	static list<ppair> neighbors;
  neighbors.clear();
	getNNearestNeighbors(&neighbors, p, 5);
	Vector3f center;
	Vector3f veltotal;
	for(list<ppair>::iterator i = neighbors.begin(); i != neighbors.end(); ++i) {
    //  cout << i->point->pos << " " << i->point->vel << endl;
		center += i->point->pos;
		veltotal += i->point->vel;
	}
  
  float size = neighbors.size();
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

priority_queue<ppair> Group::calcDistances(particle* p1) {
	priority_queue<ppair> distances;
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		particle* p2 = *iter;
		if(p1 != p2) {
			distances.push(ppair(point_distance(p1, p2), p2));
		}
	}
	return distances;
}

void Group::getNNearestNeighbors(list<ppair>* neighbors, particle* p1, int num) {
  if(num > MAX_KNN_SIZE) {
    printf("knn searching for too many points");
    exit(1);
  }
  
  kdTree->annkSearch(             // search
        (ANNcoord*)&p1->pos,      // query point
        num,                      // number of near neighbors
        this->nnIdx,              // nearest neighbors (returned)
        this->dists,              // distance (returned)
        0);                       // error bound
  
  for(int i = 0; i < num; ++i) {
    neighbors->push_back(ppair(sqrt(this->dists[i]), (*this)[this->nnIdx[i]]));
  }
}

// return a list of all neighbors to point p1 which are most max_dist away 
// sorted by proximity 
void Group::getNearestNeighbors(list<ppair>* neighbors, particle* p1, float max_dist) {
	priority_queue<ppair> distances = calcDistances(p1);
	
	for(vector<particle*>::iterator iter = this->begin(); iter != this->end(); ++iter) {
		particle* p2 = *iter;
		if(p1 != p2) {
			distances.push(ppair(point_distance(p1, p2), p2));
		}
	}
	
	// priority_queue<ppair>::const_iterator iter = distances.begin();
	while( true ) {
		ppair point = distances.top();
		if(point.distance < max_dist) {
			neighbors->push_back(point);
			distances.pop();
		} else {
			break;
		}
	}
	
//  	printf("distances: %d\n", distances.size());
}


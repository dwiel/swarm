#ifndef GROUP_HPP
#define GROUP_HPP

#include <set>
#include <list>
#include <queue>
#include <vector>

#include <ANN/ANN.h>          // ANN declarations

#include "vmath.h"

#include "scene.hpp"
#include "particle.hpp"

using namespace std;

struct ppair {
	float distance;
	particle* point;
  
  ppair() {
    this->distance = 0;
    this->point = 0;
  }
	
	ppair(float _distance, particle* _point) {
		this->distance = _distance;
		this->point = _point;
	}
};

bool operator<(const ppair& p1, const ppair& p2);

class Group: public vector<particle*> {
public:
	Group();
  ~Group();
	
	void Draw();
	void Move(float timediff);
	
	void figureVelocities();

// 	list<ppair> getNearestNeighbors(int p1,  int num);
	Vector3f moveToNeighborsCenter(particle* p);
	Vector3f stayInBounds(particle* p);
	priority_queue< ppair > calcDistances(particle* p1);
	void getNearestNeighbors(list<ppair>*, particle* p1, float max_dist);
	void getNNearestNeighbors(vector<ppair>*, particle* p1, int max_dist);
	Vector3f avoidTouching(particle* p);

	priority_queue<ppair> recent_distances;
  
  void buildKNN();
  void destroyKNN();
  void resizeDataPts();

	float maximum_velocity;
	float speed;
	bool pause_movement;
  bool controlled;
  float color_off;
  
  float vel_render_scale_x;
  float vel_render_scale_y;
  
  float vel_render_base_size_x;
  float vel_render_base_size_y;
  
  float move_to_neighbor_center_weight;
  float stay_in_bounds_weight;
  float avoid_touching_weight;
  
  Vector3f pos;
	
	Scene *scene;

  vector<ppair> neighbors;
  
  ANNpointArray   dataPts;        // data points
  int             dataPtsSize;    // number of data points
  ANNpoint        queryPt;        // query point
  ANNidxArray     nnIdx;          // near neighbor indices
  ANNdistArray    dists;          // near neighbor distances
  ANNkd_tree*     kdTree;         // search structure
};

#endif // GROUP_HPP
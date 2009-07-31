#ifndef GROUP_HPP
#define GROUP_HPP

#include <set>
#include <list>
#include <queue>
#include <vector>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

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

  // public functions
	void Draw();
	void Move(float timediff);
	
  // public parameters
  bool render;
  float maximum_velocity;
  float speed;
  bool pause_movement;
  bool controlled;
  float color_off;
  float vel_render_scale_x;
  float vel_render_scale_y;
  float render_base_size_x;
  float render_base_size_y;
  float move_to_neighbor_center_weight;
  float stay_in_bounds_weight;
  float avoid_touching_weight;
	float decay;
  Vector3f pos;
  Scene *scene;
  enum RenderType {
    RT_normal,
    RT_grid,
    RT_sphere
  } rendertype;
  float gridsize;
  
  static set<Group*> groups;

private:
  // internal functions
	void figureVelocities();
// 	list<ppair> getNearestNeighbors(int p1,  int num);
	Vector3f moveToNeighborsCenter(particle* p);
	Vector3f stayInBounds(particle* p);
	priority_queue< ppair > calcDistances(particle* p1);
	void getNearestNeighbors(list<ppair>*, particle* p1, float max_dist);
	void getNNearestNeighbors(vector<ppair>*, particle* p1, int max_dist);
	Vector3f avoidTouching(particle* p);

  // internal data
  vector<ppair> neighbors;  

  // knn helper and storage
  void buildKNN();
  void destroyKNN();
  void resizeDataPts();
  ANNpointArray   dataPts;        // data points
  unsigned int    dataPtsSize;    // number of data points
  ANNpoint        queryPt;        // query point
  ANNidxArray     nnIdx;          // near neighbor indices
  ANNdistArray    dists;          // near neighbor distances
  ANNkd_tree*     kdTree;         // search structure

  struct particle* particles;
  int num_particles;
	
  static boost::mt19937 rng;
};

#endif // GROUP_HPP
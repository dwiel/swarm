/*
 *		This Code Was Created By Jeff Molofee 2000
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 *
 *		this linux port by Ken Rockot ( read README )
 */

#include <stdlib.h>
#include <stdio.h>					// Header File For Standard Input/Output
#include <GL/gl.h>					// Header File For The OpenGL 
#include <GL/glu.h>					// Header File For The GLu
#include <sys/time.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include <list>
#include <vector>
#include <iostream>
#include <queue>

#include <math.h>

using namespace std;

#include "SDL.h"

#include "lo/lo.h"

#include "vmath.h"

#include "scene.hpp"
#include "particle.hpp"
#include "group.hpp"

#define	MAX_PARTICLES 100          // Number Of Particles To Create
#define MAX_GROUPS 1

bool keys[512];

bool active = true;                // Window Active Flag Set To TRUE By Default
bool fullscreen = true;            // Fullscreen Flag Set To Fullscreen Mode By Default
bool rainbow = true;			         // Rainbow Mode?
bool sp;                           // Spacebar Pressed?
bool rp;                           // Enter Key Pressed?
int pause_movement = 0;

float	slowdown = 0.1f;             // Slow Down Particles
float	xspeed;                      // Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;                      // Base Y Speed (To Allow Keyboard Direction Of Tail)
float	zoom = -40.0f;               // Used To Zoom Out

Scene scene;

GLuint	loop;                      // Misc Loop Variable
GLuint	col;                       // Current Color Selection
GLuint	delay;                     // Rainbow Effect Delay
GLuint	texture[1];                // Storage For Our Particle Texture

struct particle particles[MAX_PARTICLES * MAX_GROUPS]; // Particle Array (Room For Particle Info)

Group groups[MAX_GROUPS];

static GLfloat colors[12][3] = {   // Rainbow Of Colors
	{1.0f,0.5f,0.5f}, {1.0f,0.75f,0.5f}, {1.0f,1.0f,0.5f}, {0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f}, {0.5f,1.0f,0.75f}, {0.5f,1.0f,1.0f}, {0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f}, {0.75f,0.5f,1.0f}, {1.0f,0.5f,1.0f}, {1.0f,0.5f,0.75f}
};

int HandleSDL ( SDL_Event *event );					// SDL event handler

int LoadGLTextures()							// Load Bitmap And Convert To A Texture
{
	int Status;
	GLubyte *tex = new GLubyte[32 * 32 * 3];
	FILE *tf;

	tf = fopen ( "data/particle.raw", "rb" );
	fread ( tex, 1, 32 * 32 * 3, tf );
	fclose ( tf );

	// do stuff
	Status=1;						// Set The Status To TRUE
	glGenTextures(1, &texture[0]);				// Create One Texture

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 32, 32,
		0, GL_RGB, GL_UNSIGNED_BYTE, tex);

	delete [] tex;

  return Status;							// Return The Status
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)			// Resize And Initialize The GL Window
{
	if (height==0)							// Prevent A Divide By Zero By
	{
		height=1;						// Making Height Equal One
	}

	glViewport(0,0,width,height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);					// Select The Projection Matrix
	glLoadIdentity();						// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,20000.0f);

	glMatrixMode(GL_MODELVIEW);					// Select The Modelview Matrix
	glLoadIdentity();						// Reset The Modelview Matrix
}


int InitGL(void) {
	if(!LoadGLTextures()) {
		// If Texture Didn't Load Return FALSE
		return 0;
	}

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);        // Black Background
	glClearDepth(1.0f);                       // Depth Buffer Setup
	glDisable(GL_DEPTH_TEST);                 // Disable Depth Testing
	glEnable(GL_BLEND);                       // Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);         // Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);     // Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);  // Really Nice Point Smoothing
	glEnable(GL_TEXTURE_2D);                  // Enable Texture Mapping
	glBindTexture(GL_TEXTURE_2D, texture[0]); // Select Our Texture

	boost::mt19937 rng;
	boost::normal_distribution<float> ndist(0, 5);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > normr(rng, ndist);

	// initialize all particles
  for(int i = 0; i < MAX_GROUPS; ++i) {
    for(loop = 0; loop < MAX_PARTICLES; ++loop)	{
      particles[i*MAX_PARTICLES + loop].active = true;
      particles[i*MAX_PARTICLES + loop].life = 1.0f;
      particles[i*MAX_PARTICLES + loop].pos = Vector3f(normr()+(i*10), normr(), normr());
      //particles[i*MAX_PARTICLES + loop].vel = Vector3f(normr(), normr(), normr());
      particles[i*MAX_PARTICLES + loop].vel = Vector3f(normr()*0, normr()*0, normr()*0);
      groups[i].push_back(&particles[i*MAX_PARTICLES + loop]);
    }
    groups[i].scene = &scene;
    groups[i].controlled = true;
    groups[i].pos = Vector3f(i*10,0,0);
  }
	
// 	for(;loop < MAX_PARTICLES; ++loop)	{
// 		particles[loop].active = true;
// 		particles[loop].life = 1.0f;
// 		particles[loop].pos = Vector3f(normr(), normr(), normr());
// 		groups[1].insert(&particles[loop]);
// 	}
// 	groups[1].scene = &scene;
	
	// Initialization Went OK
	return 1;
}

int DrawGLScene(double timediff)							// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity();						// Reset The ModelView Matrix
	
	for(loop = 0; loop < MAX_GROUPS; ++loop) {
		groups[loop].Draw();
	}
	
	SDL_GL_SwapBuffers ();

	return 1;									// Everything Went OK
}

void print_debug() {
//   printf("number of 0s"
}

void control_group_key(int group, bool toggle_control_group) {
  if(toggle_control_group) {
    groups[group].controlled = !groups[group].controlled;
  } else {
    for(int i = 0; i < MAX_GROUPS; ++i) {
      groups[i].controlled = false;
    }
    groups[group].controlled = true;
  }
}

void check_keys (float timediff)
{
	//if (keys[SDLK_]) pause_movement ^= 1;
  bool toggle_control_group = false;
  if (keys[SDLK_LCTRL] || keys[SDLK_RCTRL])
    toggle_control_group = true;
    
  if (keys[SDLK_1]) control_group_key(0, toggle_control_group);
  if (keys[SDLK_2]) control_group_key(1, toggle_control_group);
  if (keys[SDLK_3]) control_group_key(2, toggle_control_group);
  if (keys[SDLK_4]) control_group_key(3, toggle_control_group);
  if (keys[SDLK_5]) control_group_key(4, toggle_control_group);
  if (keys[SDLK_6]) control_group_key(5, toggle_control_group);
  if (keys[SDLK_7]) control_group_key(6, toggle_control_group);
  if (keys[SDLK_8]) control_group_key(7, toggle_control_group);
  
  if (keys[SDLK_d]) print_debug();

  // maybe wait and do this in Lua or python ...
  // controls.push_back(Control(&scene.speed, "scene speed", Linear(SDLK_s, 0.1f));
  
  // TODO: control amount velocity affects shape
  // TODO: control weight on nearest neighbors
  // TODO: control number of particles in swarm

  if (keys[SDLK_s] && keys[SDLK_UP]) {
    scene.speed += scene.speed * 0.1f * timediff;
    cout << "scene speed " << scene.speed << endl;
  }
  if (keys[SDLK_s] && keys[SDLK_DOWN]) {
    scene.speed -= scene.speed * 0.1f * timediff;
    cout << "scene speed " << scene.speed << endl;
  }
  if (keys[SDLK_s] && keys[SDLK_PAGEUP]) {
    scene.speed += scene.speed * 1.0f * timediff;
    cout << "scene speed " << scene.speed << endl;
  }
  if (keys[SDLK_s] && keys[SDLK_PAGEDOWN]) {
    scene.speed -= scene.speed * 1.0f * timediff;
    cout << "scene speed " << scene.speed << endl;
  }
  
  if (keys[SDLK_b] && keys[SDLK_x] && keys[SDLK_UP]) {
    groups[0].vel_render_scale_x += groups[0].vel_render_scale_x * 0.1f * timediff;
    cout << "vel_render_scale_x: " << groups[0].vel_render_scale_x << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_x] && keys[SDLK_DOWN]) {
    groups[0].vel_render_scale_x -= groups[0].vel_render_scale_x * 0.1f * timediff;
    cout << "vel_render_scale_x: " << groups[0].vel_render_scale_x << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_x] && keys[SDLK_PAGEUP]) {
    groups[0].vel_render_scale_x += groups[0].vel_render_scale_x * 1.0f * timediff;
    cout << "vel_render_scale_x: " << groups[0].vel_render_scale_x << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_x] && keys[SDLK_PAGEDOWN]) {
    groups[0].vel_render_scale_x -= groups[0].vel_render_scale_x * 1.0f * timediff;
    cout << "vel_render_scale_x: " << groups[0].vel_render_scale_x << endl;
  }

  if (keys[SDLK_b] && keys[SDLK_y] && keys[SDLK_UP]) {
    groups[0].vel_render_scale_y += groups[0].vel_render_scale_y * 0.1f * timediff;
    cout << "vel_render_scale_y: " << groups[0].vel_render_scale_y << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_y] && keys[SDLK_DOWN]) {
    groups[0].vel_render_scale_y -= groups[0].vel_render_scale_y * 0.1f * timediff;
    cout << "vel_render_scale_y: " << groups[0].vel_render_scale_y << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_y] && keys[SDLK_PAGEUP]) {
    groups[0].vel_render_scale_y += groups[0].vel_render_scale_y * 1.0f * timediff;
    cout << "vel_render_scale_y: " << groups[0].vel_render_scale_y << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_y] && keys[SDLK_PAGEDOWN]) {
    groups[0].vel_render_scale_y -= groups[0].vel_render_scale_y * 1.0f * timediff;
    cout << "vel_render_scale_y: " << groups[0].vel_render_scale_y << endl;
  }

  else if (keys[SDLK_b] && keys[SDLK_UP]) {
    groups[0].vel_render_scale_x += groups[0].vel_render_scale_x * 0.1f * timediff;
    groups[0].vel_render_scale_y += groups[0].vel_render_scale_y * 0.1f * timediff;
    cout << "vel_render_scale: " << groups[0].vel_render_scale_x << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_DOWN]) {
    groups[0].vel_render_scale_x -= groups[0].vel_render_scale_x * 0.1f * timediff;
    groups[0].vel_render_scale_y -= groups[0].vel_render_scale_y * 0.1f * timediff;
    cout << "vel_render_scale: " << groups[0].vel_render_scale_x << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_PAGEUP]) {
    groups[0].vel_render_scale_x += groups[0].vel_render_scale_x * 1.0f * timediff;
    groups[0].vel_render_scale_y += groups[0].vel_render_scale_y * 1.0f * timediff;
    cout << "vel_render_scale: " << groups[0].vel_render_scale_x << endl;
  }
  else if (keys[SDLK_b] && keys[SDLK_PAGEDOWN]) {
    groups[0].vel_render_scale_x -= groups[0].vel_render_scale_x * 1.0f * timediff;
    groups[0].vel_render_scale_y -= groups[0].vel_render_scale_y * 1.0f * timediff;
    cout << "vel_render_scale: " << groups[0].vel_render_scale_x << endl;
  }
  
  if (keys[SDLK_c] && keys[SDLK_UP]) {
    groups[0].color_off += 1.0f * timediff;
    cout << "color_off: " << groups[0].color_off << endl;
  }
  if (keys[SDLK_c] && keys[SDLK_DOWN]) {
    groups[0].color_off -= 1.0f * timediff;
    cout << "color_off: " << groups[0].color_off << endl;
  }
  if (keys[SDLK_c] && keys[SDLK_PAGEUP]) {
    groups[0].color_off += 10.0f * timediff;
    cout << "color_off: " << groups[0].color_off << endl;
  }
  if (keys[SDLK_c] && keys[SDLK_PAGEDOWN]) {
    groups[0].color_off -= 10.0f * timediff;
    cout << "color_off: " << groups[0].color_off << endl;
  }
  
	if (keys[SDLK_z] && keys[SDLK_UP]) {
    scene.zoom += 1.0f * timediff;				// Zoom In
    cout << "scene zoom: " << scene.zoom << endl;
  }
	if (keys[SDLK_z] && keys[SDLK_DOWN]) {
    scene.zoom -= 1.0f * timediff;			// Zoom Out
    cout << "scene zoom: " << scene.zoom << endl;
  }
  if (keys[SDLK_z] && keys[SDLK_PAGEUP]) {
    scene.zoom += 10.0f * timediff;        // Zoom In
    cout << "scene zoom: " << scene.zoom << endl;
  }
  if (keys[SDLK_z] && keys[SDLK_PAGEDOWN]) {
    scene.zoom -= 10.0f * timediff;      // Zoom Out
    cout << "scene zoom: " << scene.zoom << endl;
  }
}

int HandleSDL(SDL_Event *event)
{
	int done = 0;

	switch(event->type)
	{
		case SDL_QUIT:
			done = 1;
			break;

		case SDL_KEYDOWN:
			if(event->key.keysym.sym == SDLK_ESCAPE) done = 1;
			keys[event->key.keysym.sym] = true;
			break;

		case SDL_KEYUP:
			keys[event->key.keysym.sym] = false;
			break;
	}

	return done;
}

void OSC_error(int num, const char *msg, const char *path) {
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
}

// 100 - 600
int bass_handler(const char *path, const char *types, lo_arg **argv, int argc,
     void *data, void *user_data)
{
    groups[0].vel_render_scale_x = (argv[0]->f / 100.0f) - 1.0f;
    groups[0].vel_render_scale_y = (argv[0]->f / 100.0f) - 1.0f;

    return 0;
}

// - 1000
int mid_handler(const char *path, const char *types, lo_arg **argv, int argc,
     void *data, void *user_data)
{
    groups[0].color_off = (argv[0]->f / 50.0f);

    return 0;
}

// - 1500
int high_handler(const char *path, const char *types, lo_arg **argv, int argc,
     void *data, void *user_data)
{
    groups[0].scene->speed = pow(argv[0]->f, 2.0f) / 1500;

    return 0;
}



int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);
  lo_server s = lo_server_new("9001", OSC_error);
  lo_server_add_method(s, "/notes/bass", "f", bass_handler, NULL);
  lo_server_add_method(s, "/notes/mid", "f", mid_handler, NULL);
  lo_server_add_method(s, "/notes/high", "f", high_handler, NULL);


	int flags = SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_OPENGL;
	// int flags = SDL_FULLSCREEN | SDL_OPENGL;
	//int flags = SDL_OPENGL;
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_SetVideoMode(1024, 768, 16, flags);
	// SDL_SetVideoMode(640, 480, 16, flags);

	InitGL ();
	ReSizeGLScene ( 1024, 768 );
	// ReSizeGLScene ( 640, 480 );

	timeval now, prev;
	double timediff;
	int done = 0;
	gettimeofday(&prev, NULL); // intialize the time
	while(!done)
	{
		gettimeofday(&now, NULL);
		timediff = (now.tv_sec - prev.tv_sec) + (now.tv_usec - prev.tv_usec)/1000000.0f;
		prev = now;
 		printf("\rFPS: %f", 1.0f/timediff);
 		for(int i = 0; i < MAX_GROUPS; ++i) {
 			groups[i].Move(timediff);
 		}
		//groups[0].Move(timediff);
		DrawGLScene(timediff);
		check_keys(timediff);
		
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			done = HandleSDL(&event);
		}
    
    lo_server_recv_noblock(s, 0);
	}

	SDL_Quit();
  
  cout << endl;
}

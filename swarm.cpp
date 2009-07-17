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

#include <list>
#include <vector>
#include <iostream>
#include <queue>

#include <math.h>

using namespace std;

#include "SDL.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "lo/lo.h"

#include "vmath.h"

#include "scene.hpp"
#include "particle.hpp"
#include "group.hpp"

#include "swarm.hpp"

#include <tolua.h>
#include "tolua_group.h"
#include "tolua_swarm.h"
#include "tolua_vmath.h"

lua_State *L;

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

bool keys[512];

#define MAX_GROUPS 0

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
	size_t ret = fread ( tex, 1, 32 * 32 * 3, tf );
	fclose ( tf );
  
  if(ret) {
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
  } else {
    return 0;
  }
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

  for(set<Group*>::iterator iter = Group::groups.begin(); iter != Group::groups.end(); ++iter) {
    (*iter)->scene = &scene;
    (*iter)->controlled = true;
  }
	
	// Initialization Went OK
	return 1;
}

int DrawGLScene(double timediff)							// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity();						// Reset The ModelView Matrix
	
  for(set<Group*>::iterator iter = Group::groups.begin(); iter != Group::groups.end(); ++iter) {
    (*iter)->Draw();
  }
	
	SDL_GL_SwapBuffers ();

	return 1;									// Everything Went OK
}

void print_debug() {
//   printf("number of 0s"
}

void check_keys (float timediff)
{
  if(keys[SDLK_SPACE]) luaL_dofile(L, "foo.lua");
  
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
    Group *group = (*Group::groups.begin());
    group->vel_render_scale_x = (argv[0]->f / 1000.0f) - 1.0f;
    group->vel_render_scale_y = (argv[0]->f / 1000.0f) - 1.0f;
    
    cout << "bass:" << argv[0]->f << endl;

    return 0;
}

// - 1000
int mid_handler(const char *path, const char *types, lo_arg **argv, int argc,
     void *data, void *user_data)
{
  Group *group = (*Group::groups.begin());
  group->color_off = (argv[0]->f / 50.0f);

  cout << "mid:" << argv[0]->f << endl;

  return 0;
}

// - 1500
int high_handler(const char *path, const char *types, lo_arg **argv, int argc,
     void *data, void *user_data)
{
    groups[0].scene->speed = pow(argv[0]->f, 2.0f) / 1500;

    return 0;
}

int generic_handler(const char *path, const char *types, lo_arg **argv,
        int argc, void *data, void *user_data)
{
//     int i;
// 
//     printf("path: <%s>\n", path);
//     for (i=0; i<argc; i++) {
//       printf("arg %d '%c' ", i, types[i]);
//       //lo_arg_pp(types[i], argv[i]);
//       printf("\n");
//     }
//     printf("\n");
//     fflush(stdout);
// 
//     return 1;


  lua_getfield(L, LUA_GLOBALSINDEX, "OSCevent");
  lua_pushstring(L, path);
  if(types[0] == 'f' ) {
    lua_pushnumber(L, argv[0]->f);
  } else {
    lua_pushnumber(L, argv[0]->i);
  }
  ;
  if (lua_pcall(L, 2, 0, 0)) {
    cout << "error in OSCevent: " << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }

  return 1;
}


int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);
  lo_server s = lo_server_new("9001", OSC_error);
//   lo_server_add_method(s, "/notes/bass", "f", bass_handler, NULL);
//   lo_server_add_method(s, "/notes/mid", "f", mid_handler, NULL);
  //lo_server_add_method(s, "/notes/high", "f", high_handler, NULL);
  lo_server_add_method(s, NULL, NULL, generic_handler, NULL);


	//int flags = SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_OPENGL;
	// int flags = SDL_FULLSCREEN | SDL_OPENGL;
	int flags = SDL_OPENGL;
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	// SDL_SetVideoMode(1024, 768, 16, flags);
	SDL_SetVideoMode(640, 480, 16, flags);

	InitGL ();
	// ReSizeGLScene ( 1024, 768 );
	ReSizeGLScene ( 640, 480 );
  
  L = lua_open();
  luaL_openlibs(L);
  tolua_group_open(L);
  tolua_swarm_open(L);
  tolua_vmath_open(L);
  luaL_dofile(L, "foo.lua");

  lua_getfield(L, LUA_GLOBALSINDEX, "init");
  lua_pcall(L, 0, 0, 0);
  
  for(set<Group*>::iterator iter = Group::groups.begin(); iter != Group::groups.end(); ++iter) {
    (*iter)->scene = &scene;
    (*iter)->controlled = true;
  }
  
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
    for(set<Group*>::iterator iter = Group::groups.begin(); iter != Group::groups.end(); ++iter) {
 			(*iter)->Move(timediff);
 		}
		DrawGLScene(timediff);
		check_keys(timediff);
    
    lua_getfield(L, LUA_GLOBALSINDEX, "update");
    if (lua_pcall(L, 0, 0, 0) != 0) {
      cout << "error in OSCevent: " << lua_tostring(L, -1) << endl;
      lua_pop(L, 1);
    }
		
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			done = HandleSDL(&event);
		}
    
    lo_server_recv_noblock(s, 0);
	}

	SDL_Quit();
  lua_close(L);
  
  cout << endl;
}

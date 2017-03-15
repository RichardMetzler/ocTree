/**********************************************************************

  Arcball.h

  A C++ class that implements the Arcball, as described by Ken
  Shoemake in Graphics Gems IV.
  This class takes as input mouse events (mouse down, mouse drag,
  mouse up), and creates the appropriate quaternions and 4x4 matrices
  to represent the rotation given by the mouse.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  This class is used as follows:
  - initialize [either in the constructor or with set_params()], the
    center position (x,y) of the arcball on the screen, and the radius
  - on mouse down, call mouse_down(x,y) with the mouse position
  - as the mouse is dragged, repeatedly call mouse_motion() with the
    current x and y positions.  One can optionally pass in the current
    state of the SHIFT, ALT, and CONTROL keys (passing zero if keys
    are not pressed, non-zero otherwise), which constrains
    the rotation to certain axes (X for CONTROL, Y for ALT).
  - when the mouse button is released, call mouse_up()
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Axis constraints can also be explicitly set with the
  set_constraints() function.

  The current rotation is stored in the 4x4 float matrix 'rot'.
  It is also stored in the quaternion 'q_now'.

  ------------------------------------------------------------------

  Feb 25, 1998 - Paul Rademacher (rademach@cs.unc.edu)

**********************************************************************/


#ifndef _ARCBALL_H_
#define _ARCBALL_H_

#include <math.h>
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

//using namespace glm ;

class Arcball {

public:
  bool  constraint_x, constraint_y;
  glm::vec2  center;
  float radius, damp_factor;
  int   zero_increment;

  glm::vec3  constrain_vector( glm::vec3 vector, glm::vec3 axis );
  glm::vec3  mouse_to_sphere( glm::vec2 p );

  //public:
    int   is_mouse_down;  /* true for down, false for up */
    int   is_spinning;
    glm::quat q_now, q_down, q_drag, q_increment;
    glm::vec2  down_pt;
    glm::mat4  rot, rot_increment;
   glm:: mat4  *rot_ptr;

    void  set_damping( float d );
    void  idle( void );
    void  mouse_down( int x, int y );
    void  mouse_up( void );
    void  mouse_motion( int x, int y, int shift, int ctrl, int alt );
    void  mouse_motion( int x, int y );
    void  set_constraints( bool constrain_x, bool constrain_y );
    void  set_params( glm::vec2 center, float radius );
    void  reset_mouse( void );
    void  init( void );

    Arcball( void );
    Arcball( glm::mat4 *mtx );
    Arcball( glm::vec2 center, float radius );
};


#endif
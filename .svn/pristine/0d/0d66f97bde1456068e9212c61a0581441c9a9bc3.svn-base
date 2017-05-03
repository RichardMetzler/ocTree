/**********************************************************************

  arcball.cpp


          --------------------------------------------------

  Copyright (c) 1998 Paul Rademacher

  This program is freely distributable without licensing fees and is
  provided without guarantee or warrantee expressed or implied. This
  program is -not- in the public domain.

**********************************************************************/


#include "arcball.h"
#include <stdio.h>


/**************************************** Arcball::Arcball() ****/
/* Default (void) constructor for Arcball                         */

//using namespace glm;

Arcball::Arcball( void )
{
  rot_ptr = &rot;

  init();
}


/**************************************** Arcball::Arcball() ****/
/* Takes as argument a glm::mat4 to use instead of the internal rot  */

Arcball::Arcball( glm::mat4 *mtx )
{
  rot_ptr = mtx;
}


/**************************************** Arcball::Arcball() ****/
/* A constructor that accepts the screen center and arcball radius*/

Arcball::Arcball( glm::vec2 _center, float _radius )
{
  rot_ptr = &rot;

  init();
  set_params( _center, _radius );
}


/************************************** Arcball::set_params() ****/

void Arcball::set_params( glm::vec2 _center, float _radius )
{
  center      = _center;
  radius      = _radius;
}


/*************************************** Arcball::init() **********/

void Arcball::init( void )
{
  center = glm::vec2( 0.0, 0.0 );
  radius        = 1.0;
  q_now         = glm::quat(1.,0.,0.,0.);//glm::quat_identity();
  *rot_ptr      = glm::mat4(1.0);//identity3D();
  q_increment   = glm::quat(1.,0.,0.,0.);//glm::quat_identity();
  rot_increment = glm::mat4(1.0);//identity3D();
  is_mouse_down = false;
  is_spinning   = false;
  damp_factor   = 0.0;
  zero_increment = true;
}


/*********************************** Arcball::mouse_to_sphere() ****/
#include<iostream>
glm::vec3 Arcball::mouse_to_sphere( glm::vec2 p )
{
  float mag;
  glm::vec2  v2 = (p - center) / radius;
  glm::vec3  v3( v2[0], v2[1], 0.0 );
  
//  glm::vec3  axis;

  mag = glm::dot(v2, v2);

  if ( mag > 1.0f ) {
    glm::normalize(v3);
  }
  else {
    v3[2] = sqrt( 1.0f - mag );
  }

  /* Now we add constraints - X takes precedence over Y */
  if ( constraint_x ) {
    v3 = constrain_vector( v3, glm::vec3( 1.0, 0.0, 0.0 ));
  } else if ( constraint_y ) {
    v3 = constrain_vector( v3, glm::vec3( 0.0, 1.0, 0.0 ));
  }

  return v3;
}


/************************************ Arcball::constrain_vector() ****/

glm::vec3 Arcball::constrain_vector( glm::vec3 vector, glm::vec3 axis )
{
  return glm::normalize(vector-(vector*axis)*axis);
}

/************************************ Arcball::mouse_down() **********/

void Arcball::mouse_down( int x, int y )
{
  down_pt = glm::vec2((float)x, (float) y );
  is_mouse_down = true;

  q_increment   = glm::quat(1.,0.,0.,0.);
  rot_increment = glm::mat4(1.);
  zero_increment = true;
}


/************************************ Arcball::mouse_up() **********/

void Arcball::mouse_up( void )
{
  q_now = q_drag * q_now;
  is_mouse_down = false;
}


/********************************** Arcball::mouse_motion() **********/

void Arcball::mouse_motion( int x, int y, int shift, int ctrl, int alt )
{
  /* Set the X constraint if CONTROL key is pressed, Y if ALT key */
  set_constraints( ctrl != 0, alt != 0 );

  glm::vec2 new_pt( (float)x, (float) y );
  glm::vec3 v0 = mouse_to_sphere( down_pt );
  glm::vec3 v1 = mouse_to_sphere( new_pt );

  glm::vec3 cross = glm::cross(v0,v1);

  q_drag = glm::quat(glm::dot(v0, v1) ,glm::vec3(cross.x,cross.y,cross.z));

  //    *rot_ptr = (q_drag * q_now).to_glm::mat4();
  glm::mat4 tmp = glm::mat4_cast(q_drag);//.to_glm::mat4();  // avoid warning in following line
  *rot_ptr = (*rot_ptr) * tmp;  // operator *() takes references
  down_pt = new_pt;

        /* We keep a copy of the current incremental rotation (= q_drag) */
  q_increment   = q_drag;
  rot_increment = glm::mat4_cast(q_increment);//.to_glm::mat4();

  set_constraints( false, false );

  if ( q_increment.w < .999999 ) {
    is_spinning = true;

    zero_increment = false;
  }
  else {
    is_spinning = false;
    zero_increment = true;
  }
}


/********************************** Arcball::mouse_motion() **********/

void Arcball::mouse_motion( int x, int y )
{
  mouse_motion( x, y, 0, 0, 0 );
}


/***************************** Arcball::set_constraints() **********/

void Arcball::set_constraints(bool _constraint_x, bool _constraint_y )
{
  constraint_x = _constraint_x;
  constraint_y = _constraint_y;
}

/***************************** Arcball::idle() *********************/

void  Arcball::idle( void )
{
  if ( is_mouse_down ) {
    is_spinning = false;
    zero_increment = true;
  }

  if ( damp_factor < 1.0 ) {
	  //std::cout<<" in damping "<<q_increment.w<<" , "<<q_increment.x<<" , "<<q_increment.y<<" , "<<q_increment.z<<std::endl;
      float scale;
      glm::vec3 axis;
	  
       scale = (float)sin( acos( q_increment.w ) );
        if ( scale < .00001 && scale > -.00001 )
            axis = glm::vec3( 0.0, 0.0, 0.0 );
        else
            axis = glm::vec3(q_increment.x,q_increment.y, q_increment.z) / scale;

        float angle = glm::radians((1.0f - damp_factor) * (float)glm::degrees( 2.0f * acos( q_increment.w ) )/2.f);
        q_increment = glm::normalize(glm::quat(cos(angle),axis * (float)sin(angle)));
		//std::cout<<" new "<<q_increment.w<<" , "<<q_increment.x<<" , "<<q_increment.y<<" , "<<q_increment.z<<std::endl;
		if(isnanf(q_increment.w)){
			q_increment = glm::quat();
		}
		 glm::mat4 tmp = glm::mat4_cast(q_increment);//.to_glm::mat4();  // avoid warning in following line
		*rot_ptr = (*rot_ptr) * tmp;


//      q_increment.w
//    q_increment.scale_angle( 1.0 - damp_factor );
  }
   
  rot_increment = glm::mat4_cast(q_increment);

  if ( q_increment.w >= .999999 ) {
    is_spinning = false;
    zero_increment = true;
  }
}


/************************ Arcball::set_damping() *********************/

void  Arcball::set_damping( float d )
{
  damp_factor = d;
}

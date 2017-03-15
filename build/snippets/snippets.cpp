#ifdef WIN32
	#include <windows.h>
#endif 
#include <iostream>
#include <string>


//#define GLEW_STATIC
#include <GL/glew.h>
#include <gl/GL.h>
#include <GL/glut.h>
#include <GL/freeglut.h>


#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#define FREEIMAGE_LIB
#include <FreeImage.h>
#include <arcball.h>
void resize(int w, int h);






int main(/* ONLY FOR AVOIDING WARNINGS */){



/* ################### - 0.1 - ################### */
		
const GLfloat colours[] = {
    1., 0.,  0.,
	0., 1.,  0.,
	0., 0.,  1.,

	1., 0.,  0.,
	0., 0.,  1.,
	1., 0.,  1.,


	0., 1.,  0.,
	0., 1.,  1.,
	1., 1.,  1.,

	0., 1.,  0.,
	1., 1.,  1.,
	0., 0.,  1.,


	0., 0.,  1.,
	1., 1.,  1.,
	1., 0.,  1.,

	0., 0.,  1.,
	1., 1.,  1.,
	1., 1.,  1.,


	1., 1.,  0.,
	1., 0.,  0.,
	1., 0.,  1.,

	1., 0.,  1.,
	1., 1.,  1.,
	1., 1.,  0.,


	1., 0.,  0.,
	1., 1.,  0.,
	0., 1.,  1.,

	1., 0.,  0.,
	0., 1.,  1.,
	0., 1.,  0.,


	1., 1.,  1.,
	0., 1.,  1.,
	1., 1.,  1.,

	1., 1.,  1.,
	0., 1.,  1.,
	1., 1.,  0.

};

/* ################### - 0.2 - ################### */

	GLuint colourBuffer; // Declare colour buffer

/* ################### - 0.3 - ################### */
	  
    glGenBuffers(1, &colourBuffer); // generating 1 buffer, put resulting identifier in this buffer
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer); 
    glBufferData(GL_ARRAY_BUFFER, 108*sizeof(GLfloat), colours, GL_STATIC_DRAW);

/* ################### - 0.4 - ################### */
	
	glEnableVertexAttribArray(1);  
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

/* ################### - 0.5 - ################### */

	glDisableVertexAttribArray(1);



/* ################### - 1.1 - ################### */

const GLfloat vertices_indexed[] = {
/*0*/    -1., -1.,  1.,
/*1*/     1., -1.,  1.,
/*2*/	  1.,  1.,  1.,
/*3*/	 -1.,  1.,  1.,

/*4*/    -1., -1., -1.,
/*5*/     1., -1., -1.,
/*6*/	  1.,  1., -1.,
/*7*/	 -1.,  1., -1.
};

const GLfloat colours_indexed[] = {
/*0*/   1., 0.,  0.,
/*1*/ 	0., 1.,  0.,
/*2*/ 	0., 0.,  1.,
/*3*/ 	1., 0.,  1.,

/*4*/ 	1., 1.,  0.,
/*5*/ 	0., 1.,  1.,
/*6*/ 	1., 1.,  1.,
/*7*/ 	1., 1.,  1.
};

const GLushort indices[] = {
	0, 1, 2, //front
	0, 2, 3,

	1, 5, 6, //right
	1, 6, 2,

	2, 7, 3, //top
	2, 6, 7,

	4, 0, 3, //left
	3, 7, 4,

	0, 4, 5, //bottom
	0, 5, 1,

	6, 5, 7, //back
	7, 5, 4
};



/* ################### - 1.2 - ################### */

GLuint indexBuffer; // Declare index buffer

/* ################### - 1.3 - ################### */

    glGenBuffers(1, &indexBuffer); // generating 1 buffer, put resulting identifier in variable indexBuffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLushort), indices, GL_STATIC_DRAW);

//* ################### - 1.4 - ################### */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

/* ################### - 1.5 - ################### */

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);


/* ################### - 2.1 - ################### */

	int old_mouse_x = 0;
	int old_mouse_y = 0;

	int left_button_state = 0;
	int middle_button_state = 0;


/* ################### - 2.2 - ################### */
	glutMotionFunc(motionFunc);

	glutMouseFunc(mouseFunc);




	
/* ################### - 2.3 - ################### */

void motionFunc(int x, int y){

	
	if (middle_button_state){
		translation[0] += float(x - old_mouse_x) / float(max_window_width) *  screen_width ;
		translation[1] += -float(y - old_mouse_y) /float(max_window_height) *  screen_height;
		old_mouse_x = x;
		old_mouse_y = y;
	} 
	glutPostRedisplay();
	/* ################### - 3.5 - ################### */
}




/* ################### - 2.4 - ################### */
void mouseFunc ( int Button, int state, int x, int y){
	if (Button == 3){
		translation[2] += 1.f;
	} else if (Button == 4){
		translation[2] += -1.f;
	} else if(Button == GLUT_MIDDLE_BUTTON){
		if(state == GLUT_DOWN){
			middle_button_state = 1;
			old_mouse_x = x;
			old_mouse_y = y;
		} else {
			middle_button_state = 0;
		}
	}
	/* ################### - 3.6 - ################### */
	

}

/* ################### - 3.1 - ################### */
	Arcball arcball;

/* ################### - 3.2 - ################### */
	arcball.init();
	arcball.set_damping(.01f);
	arcball.set_params(glm::vec2(float(max_window_width)/2.,float(max_window_height)/2.f),1000.f);

/* ################### - 3.3 - ################### */
	arcball.idle();

/* ################### - 3.4 - ################### */
	model_matrix = model_matrix * arcball.rot;


/* ################### - 3.5 - ################### */

	 else if(left_button_state){
		arcball.mouse_motion(x,y);
	} 


/* ################### - 3.6 - ################### */
	else if (Button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN){
			left_button_state = 1;
			arcball.mouse_down(x,y);
		} else{
			left_button_state = 0;
			arcball.mouse_up();
		}
	}


/* ################### - 4.1 - ################### */
const GLfloat texCoords[] = {
	0.,0.,
	1.,0.,
	1.,1.,
	1.,0.,

	1.,0.,
	0.,0.,
	0.,1.,
	1.,1.
};

/* ################### - 4.2 - ################### */

	GLuint tCoordBuffer; // Declare ID holder texture coordinate buffer
	GLuint textureBuffer; // Declare ID holder for texture
	GLuint tex_location;  // Declare ID for location in shader

/* ################### - 4.3 - ################### */
	tex_location = glGetUniformLocation(shaderprogram, "texture0");

/* ################### - 4.4 - ################### */
	glGenBuffers(1, &tCoordBuffer); // generating 1 buffer, put resulting identifier in this buffer
    glBindBuffer(GL_ARRAY_BUFFER, tCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
/* ################### - 4.5 - ################### */
	glDisableVertexAttribArray(2);


/* ################### - 4.6 - ################### */
	img_texture = loadImage("../textures/uvtemplate.bmp");  // Function for loading images
	GLsizei sizeX = FreeImage_GetWidth(img_texture); // get width of image
	GLsizei sizeY = FreeImage_GetHeight(img_texture); // get height of image

/* ################### - 4.7 - ################### */
	glGenTextures(1,&textureBuffer); //Generate a texture on graphics memory
	glBindTexture(GL_TEXTURE_2D,textureBuffer); // Bind Texture to "location" GL_TEXTURE_2D
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(img_texture)); //Upload image to graphics memory
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set filter function for far away texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Set filter function for near texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Restricts Tcoord to 0 and 1 for u t-coord
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Restricts Tcoord to 0 and 1 for v t-coord
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind from GL_TEXTURE_2D
	

/* ################### - 4.8 - ################### */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureBuffer);
	glProgramUniform1i(shaderprogram,tex_location, 0);

/* ################### - 4.9 - ################### */
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind from GL_TEXTURE_2D

/* ################### - 4.10 - ################### */
	vertexsource = filetobuf("../shader/vertex_tex.vert");
    fragmentsource = filetobuf("../shader/fragment_tex.frag");

/* ################### - 4.11 - ################### */
	/* Bind attribute index 0 (vertices) to in_Position and attribute index 1 (color) to in_Color */
    /* Attribute locations must be setup before calling glLinkProgram. */
    glBindAttribLocation(shaderprogram, 0, "in_Position");
    glBindAttribLocation(shaderprogram, 1, "in_Color");
	glBindAttribLocation(shaderprogram, 2, "in_tCoord");





}


















#ifdef WIN32
#include <windows.h>
#endif 
#include <dirent.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define FREEIMAGE_LIB
#include <FreeImage.h>
#include <arcball.h>
#include <foo.h>
#include <mesh.h>
#include <object.h>
#include <ocTree.h>
#include <ocTree.h>
#include <unistd.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Forward declarations of the needed functions
void setup(void);

void cleanup(void);

void draw(void);

void reshape(int w, int h);

bool createShaders();

void keyboardFunction(unsigned char k, int x, int y);

void motionFunction (int x, int y);

void mouseFunction (int Button, int state, int x, int y);

FIBITMAP* loadImage(const std::string filename);

// Global variables


/**
 * ##############################################################
 * #################	Pfaede zu .txt Files	#################
 * #################	 Ggf. Anpassen!!		#################
 * ##############################################################
 */
// Auge draufbehalten!
const char *SelctionLogPath="/home/ri43hiq/Desktop/txt_files/selection_log.txt";
const char *ComputedSaliencyTxtPath="/home/ri43hiq/Desktop/txt_files/computed_saliency_values.txt";

// RESULTS
const char *differencesWeightedPath = "/home/ri43hiq/Desktop/txt_files/RESULTS/differences_weighted.txt";
const char *differencesPath = "/home/ri43hiq/Desktop/txt_files/RESULTS/differences_values.txt";

// RELATIV ZU Object -> UNBEDINGT ANPASSEN!
const char *AverageUserSelectionPath = "/home/ri43hiq/Desktop/txt_files/01_cow/selected_saliency_values.txt";
const char *UserSelectedSaliencyTxtPath="/home/ri43hiq/Desktop/txt_files/01_cow/selected_saliency_values.txt";
const char *UserSelectionLogFilesPath = "/home/ri43hiq/Desktop/txt_files/01_cow/";
/**
 * ##############################################################
 */


glm::mat4 proj_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

std::vector <std::pair <size_t, glm::vec3> >  verticesList;

ocTree* myOcTree;

std::set<size_t> vertexSelection;

/**
 * vector to hold difference- and weighted difference values of each vertex
 * size_t	ID					<0>
 * float	difference			<1>
 * float	differenceWeighted	<2>
 */
std::vector<std::tuple <size_t, float, float> > saliency_differences;

std::vector<std::pair<size_t, float> > saliency_values;	// temporary vector to hold pairs of [vertexID, computedSaliencyValue]

float screen_width = 30.0; //real size in cm
float screen_height = 30.0; //real size in cm
float near_clipping = 50.; //how far is the screen away from the user (in cm)
float far_clipping = 4000.; // up to where do we want to render (in cm)

float maxSearchRadius;

// SUPER WICHTIG!!!
float threshold_weighted = 0.2;
float threshold_difference = 0.001;

int max_window_width;
int max_window_height;

int old_window_x;
int old_window_y;
int old_mouse_x = 0;
int old_mouse_y = 0;

int left_button_state = 0;
int middle_button_state = 0;

glm::vec3 translation = glm::vec3(0.f,0.f,-100.f);

FIBITMAP* img_texture = 0;
GLuint shaderprogram;

GLuint vao; // Vertex array object
GLuint vertexBuffer; // Declare ID holder vertex buffer
GLuint colourBuffer; // Declare colour buffer
GLuint indexBuffer; // Declare index buffer

Arcball arcball;

GLint pvm_matrix_location;
GLint normal_matrix;
GLint vm_matrix;

Object object1;
std::vector<Mesh*> meshes;
std::vector<glm::vec3> vertices;

float circleAtOriginRadiusOne(float x);

/* #####################################################
/	Main program
/  ##################################################### */
int main(int argc, char **argv) {
	// Get maximum window size (screen resolution of primary monitor)
#ifdef WIN32
	max_window_width = GetSystemMetrics(SM_CXSCREEN);
	max_window_height = GetSystemMetrics(SM_CYSCREEN);


#else

#endif
	// PATH TO INPUT OBJECT
//	 std::string obj_path = "obj_files/teapot.obj";		// teapot
//	 std::string obj_path = "obj_files/pedal.obj";		// drum pedal
//	 std::string obj_path = "obj_files/suzie.obj";		// monkey

//	 std::string obj_path = "obj_files/testthing_n_c.ply";		// test thing
//	std::string obj_path = "/home/ri43hiq/workspace/mesh-saliency-climberpi/Mesh-Saliency-master/object/iphone_noNormals.obj";
	std::string obj_path = "obj_files/P51_FINAL.obj";


	glutInit(&argc, argv);		// First initialise GLUT
	/* ################# Call our setup function ################# */
	setup();

	max_window_height = glutGet((GLenum)GLUT_SCREEN_HEIGHT);
	max_window_width = glutGet((GLenum)GLUT_SCREEN_WIDTH);

	// initialize & load Object object1
	if(!object1.loadObject(obj_path,
		aiProcess_CalcTangentSpace       |
		aiProcess_Triangulate            )) {
			std::cout<<"not found"<<std::endl;
	} else {
		// if the file could be opened via ASSIMP, call its uploadAllMeshes() function
		object1.uploadAllMeshes();
	}
	// retrieve mesh objects from loaded object
	meshes = object1.getMeshes();


//	myOcTree = new ocTree(meshes, 100, 3, true);		// additional information will be printed during construction
	myOcTree = new ocTree(meshes, 100, 3, false);		// no additional information will be printed during construction
	std::vector<glm::vec3> vertices = myOcTree->getVertices();
	std::cout << "!!! " << vertices.size() << std::endl;

	// converting the <vec3> vector to a vector of <size_t, vec3> pairs
	for(size_t t = 0; t < vertices.size();++t){
		std::pair<size_t, glm::vec3> myPair;
		myPair = std::make_pair(t,vertices[t]);
		verticesList.push_back(myPair);
	}

	myOcTree->buildTreeRecursively(verticesList);

//	myOcTree->debugFirstVertex();
//	myOcTree->debugTreeInfo();

	std::vector<bool> check = {true,true,false,true,true,false,true,false,false};		// 110110100
	ocTree* testNodeId = myOcTree->getNodeByIdentifierArray(check, false);
//	ocTree* testNodeId = myOcTree->getNodeByIdentifierArray(check, true);

//	std::cout << "debugging testNodeId" << std::endl;
//	testNodeId->debugIdentifierasString();

//	ocTree* testNodeCo = myOcTree->getNodeByCoordinates(-1.58f,0.88f,-1.65f, maxSearchRadius, true);
//	ocTree* testNodeCo = myOcTree->getNodeByCoordinates(-1.58f,1.0f,-1.65f, 0.1f, true);


	// Call our function for creating shaders (compiling and linking)
	if(!createShaders()){
		std::cout<<"Error creating shaders"<<std::endl;
		exit(0);
	}

	maxSearchRadius = myOcTree->getMaximumSearchRadius();
	std::cout << "maxSearchRadius: " << std::to_string(maxSearchRadius) << std::endl;

	usleep(10000);

	pvm_matrix_location = glGetUniformLocation(shaderprogram, "pvm");
	normal_matrix = glGetUniformLocation(shaderprogram, "normalMatrix");
	vm_matrix = glGetUniformLocation(shaderprogram, "vmMatrix");

    glutMainLoop();    // Go into the glut main loop

	cleanup();
    return 0;
}

/* #####################################################
/	create Window, setup openGL states
/  ##################################################### */
void setup(void) {
	std::cout << circleAtOriginRadiusOne(0.0) << std::endl;
	std::cout << circleAtOriginRadiusOne(0.5) << std::endl;
	std::cout << circleAtOriginRadiusOne(1.0) << std::endl;

	/* ######################
	/ GLUT Setup
	/  ###################### */
	glutInitContextVersion (3, 3);

	// setting up display
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    // Configure Window Postion
    glutInitWindowPosition(old_window_x, old_window_y);
    // Configure Window Size
    glutInitWindowSize(1200,800);

    // Create Window (creates also a context)
    glutCreateWindow("VR LAB 02: OpenGL Recab + OpenGL Core");

	// Initialise glew
	glewExperimental = GL_TRUE; //needed as it is old!
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
	   std::cerr<<"Error: "<<glewGetErrorString(err)<<std::endl;
	} else {
	   if (GLEW_VERSION_3_3)
	   {
		  std::cout<<"Driver supports OpenGL 3.3:"<<std::endl;
	   }
	}

	{ // Ignore first error as it is within the GLEW/OpenGL relationship ;)
		GLenum glErr;
		glErr = glGetError();
	}

    glutDisplayFunc(draw); // Call to the drawing function (if glutPostDisplay is called or anything else has changed)
	glutIdleFunc(draw); // Call to drawing function (if nothing has happed, but we want animations to be drawn smoothly)

	// Call to keyboard function (on key press)
	glutKeyboardFunc(keyboardFunction);
	glutReshapeFunc(reshape);
	glutMotionFunc(motionFunction);
	glutMouseFunc(mouseFunction);

	// Set closing behaviour: If we leave the mainloop (e.g. through user input or closing the window) we continue after the function "glutMainLoop()"
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	/* ######################
	/ OpenGL Setup
	/  ###################### */

	// insert gluErrorString beneath every line here to debug error
	glClearColor(.40f,0.4f,0.4f,1.0f);
	// Set depth which is used when clearing the drawing buffer (1.0 is maximum depth)
	glClearDepth(1.0);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Disable blending
	glDisable(GL_BLEND);

	// ArcBall setup
	arcball.init();
	arcball.set_damping(.01f);
	arcball.set_params(glm::vec2(float(max_window_width)/2.,float(max_window_height)/2.f),1000.f);
	arcball.idle();
}

float circleAtOriginRadiusOne(float x) {
	float result;
	result = sqrt(1.0-pow((x-1.0),2.0));
	return result;
}

//	Clean up function
void cleanup(void){
	// Free memory for texture and deinitialise image loading library
	if(img_texture)
		FreeImage_Unload(img_texture);
	FreeImage_DeInitialise();

	//Free up graphics memory
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteProgram(shaderprogram);
	glDeleteVertexArrays(1, &vao);
}

//	Drawing function (is called for every frame)
void draw(void) {
	GLenum glErr;
/* ################### - 3.3 - ################### */
	int screen_pos_x = glutGet((GLenum)GLUT_WINDOW_X);
	int screen_pos_y = glutGet((GLenum)GLUT_WINDOW_Y);

	if(screen_pos_x != old_window_x || screen_pos_y != old_window_y){
		old_window_x = screen_pos_x;
		old_window_y = screen_pos_y;
		int size_x = glutGet((GLenum)GLUT_WINDOW_WIDTH);
		int size_y = glutGet((GLenum)GLUT_WINDOW_HEIGHT);
		reshape(size_x, size_y);
	}

    // Clear drawing buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderprogram);

	view_matrix = glm::translate(translation);
	model_matrix =	glm::scale(glm::vec3(10.,10.,10.f));
	model_matrix = model_matrix * arcball.rot;

	glm::mat4 pvmMatrix = proj_matrix * view_matrix * model_matrix;
	for (int i=0; i!=4; i++) {
		for (int j=0; j!=4; j++) {
			break;
		}
	}
//	std::cout<<"pvm:"<<"\n"<<proj_matrix[0][0]<<","<<proj_matrix[0][1]<<","<<proj_matrix[0][2]<<","<<proj_matrix[0][3]<<","<<std::endl;
	glm::mat3 vmMat = glm::transpose(glm::inverse(glm::mat3(view_matrix * model_matrix)));
	glProgramUniformMatrix4fv(shaderprogram, pvm_matrix_location, 1, GL_FALSE,glm::value_ptr(pvmMatrix));
	glProgramUniformMatrix3fv(shaderprogram, normal_matrix, 1, GL_FALSE,glm::value_ptr( vmMat ));
	glProgramUniformMatrix4fv(shaderprogram, vm_matrix, 1, GL_FALSE,glm::value_ptr(view_matrix*model_matrix));


	// Draw into the back buffer (front is displayed)
	glDrawBuffer(GL_BACK);

	// proj_matrix, view_matrix, model_matrix
	object1.drawWithoutParams();


	// object1.draw(proj_matrix, view_matrix, model_matrix);

	glutSwapBuffers();
}

/* #####################################################
/	Reshape function (is called if the window size is changed and on window creation)
/  ##################################################### */
void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); // Set our viewport to the size of our window
	int screen_pos_x = glutGet((GLenum)GLUT_WINDOW_X);
	int screen_pos_y = glutGet((GLenum)GLUT_WINDOW_Y);

	/* calculate corners of the window with respect to the center of the screen:
	/  screen_pos_* gives us the number of pixels from the upper left corner
	*/
	float left = -(screen_width/2.f -  float(screen_pos_x) / float(max_window_width) * screen_width);
	float right = left + float(w) / float(max_window_width) * screen_width;
	float top = screen_height/2.f - float(screen_pos_y) / float(max_window_height) * screen_height;
	float bottom = top - float(h) / float(max_window_height) * screen_height;

	//create new frustum
	proj_matrix = glm::frustum(left, right, bottom, top, near_clipping, far_clipping);

	glutPostRedisplay();
}

// WASD navigation
void keyboardFunction(unsigned char k, int x, int y) {
	switch (k) {
		case 27: // 27 = esc-button
			// Leave the main loop (only possible with freeglut, not glut alone. But you should use glut anyway as it is not maintained since May 2005!
			glutLeaveMainLoop();
			break;
		case 'w':
		case 'W':	// forward
			translation[2] += 0.8f;
			break;
		case 'a':
		case 'A':	// left
			translation[0] += 0.2f;
			break;
		case 's':
		case 'S':	// backwards
			translation[2] -= 0.8f;
			break;
		case 'd':
		case 'D':	// right
			translation[0] -= 0.2f;
			break;
		case 52:	// 4/left
			break;
		case 56:	// 8/up
			break;
		case 54:	// 6/right
			break;
		case 50:	// 2/down
			break;

		case 'h':
		case 'H':	// highlight a selection @FIXME testing purpose
		{
			glm::vec3 target {-1.28,1.0,-1.65};
			myOcTree->addVerticesToSelectionByCoordinates (target, maxSearchRadius, vertexSelection, false);
			float colors[4] = {1.0, 0.0, 0.0, 0.99};
			meshes[0]->changeColors(vertexSelection, colors);
			object1.updateAllMeshes();
			break;
		}

		case 'r':
		case 'R':
		{
			glm::vec3 target {-1.28,1.0,-1.60};
			std::cout << "removing vertices" << std::endl;
			myOcTree->removeVerticesFromSelectionByCoordinates(target, 0.04, vertexSelection,false);
			object1.updateAllMeshes();
			break;
		}

		case 'p':
		case 'P':
		{
			// save current selection to text file
			std::ofstream file(SelctionLogPath);
			std::string data("vertId\tx\t\ty\t\tz\n");
			file << data;

			std::set<size_t>::iterator it = vertexSelection.begin();
			for (int n=0; n != vertexSelection.size(); ++n) {
				std::stringstream line;
				line << *it << "\t" << verticesList[*it].second.x << "\t" << verticesList[*it].second.y << "\t" << verticesList[*it].second.z << "\n";
				file << line.str();
				std::advance(it, 1);
			}
			std::cout << "vertex selection printed to selection_log.txt" << std::endl;
			break;
		}


		/**
		 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 * Get vertices by ID of input text file; store them in saliency_values
		 *
		 * @INPUT_FILE: ComputedSaliencyTxtPath
		 * 		path to text file containing the computed saliency values for each vertex in this format:
		 * 		vertId	computed saliency value		[first line, different than every following line]
		 * 		0		0.0145583
		 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 */
		case 'j':
		case 'J':
		{
			bool firstline = true;
			bool ms_values = false;
			int counter = 0;
			std::cout << "J pressed" << std::endl;
			std::ifstream in (ComputedSaliencyTxtPath);
			std::string line;
			std::string line_saliency_value;

			if (in.is_open()) {

				size_t id;
				while (getline (in,line)) {
					if(firstline) {
						if (line.find("vertId") != std::string::npos) {
						    firstline = false;
							continue;		// ignore first (header) line
						}
					}

					line_saliency_value = line.substr(line.find("\t")+1, line.size());		// get computed saliency value (everything after the tab)
					line = line.substr(0, line.find("\t", 0));								// get id & make it the new content of the line
					id = (size_t)std::stoi(line);											// id vertex

					saliency_values.push_back(std::make_pair(id, std::atof(line_saliency_value.c_str())));
				}
				auto it = saliency_values.begin();

				// debug messages
//				for (int n = 0; n != saliency_values.size(); ++n) {
//					std::cout << "vertex stored, computed mesh saliency value for vertex " << saliency_values[n].first << ": " << saliency_values[n].second << std::endl;
//					std::advance(it, 1);
//				}
				/**
				 * saliency_values now temporarily holds mesh saliency values for all the vertices in the form of
				 * saliency_value[[siz_t id][float mesh_saliency_value]]
				 */
			} else {
				std::cout << "Cannot open input file.\n";
			}

//			saliency_values.clear();
			in.close();

			break;
		}


		/**
		 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 * Compute combined saliency values out of multiple user selection log files
		 * log files must been named 'u00.txt', 'u01.txt' ... and be located in
		 * 'txt_files/01_cow', 'txt_files/02_object' ...
		 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 */
		case 'k':
		case 'K':
		{
			// idCount: empty list of pairs of <vertexId, 0> to count how many times selected each vertex
			std::vector<std::pair<size_t, int>> idCount;
			// userSelectedSaliency: list of pairs of <vertexId, float> to store saliency for each vertex based on how often it was selected by users
			std::vector<std::pair<size_t, float>> userSelectedSaliency;
			DIR *dir;
			struct dirent *ent;
			int maxCount = 0;
			size_t maxSelectedVert = 0;

			auto it = saliency_values.begin();
			for (int n = 0; n != saliency_values.size(); ++n) {
				idCount.push_back(std::make_pair(saliency_values[n].first, 0));		// default: Every vertex selected 0 times
			}

			if ((dir = opendir (UserSelectionLogFilesPath)) != NULL) {
				// iterate through all files in USERSELECTIONLOGFILESPATH
				while ((ent = readdir (dir)) != NULL) {
					std::string filename (ent->d_name);
					std::string U = "u";
					// ignore files whose filename does not start with "u"
					if (U.compare(filename.substr(0, filename.find("u",0))) == 1) {
						std::cout << "opening file: " << ent->d_name << std::endl;
						char wholePath[256];
						std::strcpy (wholePath, UserSelectionLogFilesPath);
						std::strcat (wholePath, filename.c_str());
						std::ifstream in (wholePath);

						if (in.is_open()) {
							std::string line;
							// get all lines of the file
							while (getline (in,line)) {
								// ignore first ("header") line
								if (line.find("vertId") != std::string::npos) {
									continue;		// ignore first (header) line
								} else {
									// each line is the id of a selected vertex-> direct conversion to int is possible
									int vId = std::stoi(line);

									// find & keep track of maximum amount of number a single vertex was selected
									idCount[vId].second++;
									if (idCount[vId].second > maxCount) {
										maxCount = idCount[vId].second;
										maxSelectedVert = idCount[vId].first;
									}
								}
							}

						} else {
							std::cout << "ERROR could not open " << wholePath << std::endl;
						}
						in.close();
					}
				}
				std::cout << "Vertex Nr. " << maxSelectedVert << " was selected " << maxCount << " times, which makes it the most popular vertex" << std::endl;
				closedir (dir);
			} else {
				perror ("could not open directory");
			}

			std::cout << "max number of selections for one or more vertices: " << maxCount << std::endl;

			// finally, print average user selected saliencies to txt file
			std::ofstream out(AverageUserSelectionPath);
			std::string line;

			line = "vertexId\tuser saliency";
			out << line << std::endl;

			for (int n = 0; n != saliency_values.size(); ++n) {
				line = std::to_string(n);
				line.append("\t");
				float value = (float)idCount[n].second/maxCount;
				line.append(std::to_string(value));
				out << line << std::endl;
			}

			break;
		}


		/**
		 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 * Read from input files the computed well as the user selected combined saliency values for each vertex pi
		 * compute difference as absolute value (diff²)
		 * compute average computed saliency value of vertices surrounding pi
		 * if neighboring saliency is greater than MEAN_NEIGHBOR_SALIENCY, multiply diff by FACTOR
		 *
		 *	write total combined saliency to output file
		 *
		 * @PARAM FACTOR:					123
		 * @PARAM MEAN_NEIGHBOR_SALIENCY:	0.2
		 *
		 * @INPUT_FILE:
		 * 		path	/txt_files/computed_saliency_values.txt
		 * 		file containing computed saliency values
		 *
		 * @INPUT_FILE:
		 * 		path	/txt_files/01_cow/selected_saliency_values.txt		// KEEP UP TO DATE!
		 * 		file containing user selected saliency values
		 *
		 *
		 * @OUTPUT_FILE:
		 * 		path 	/txt_files/RESULTS/differences_values.txt
		 * 		output file containing differences
		 *
		 * @OUTPUT_FILE:
		 * 		path 	/txt_files/RESULTS/differences_weighted.txt
		 * 		output file containing final weighted differences
		 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		 */
		case 'l':
		case 'L':
		{
			bool firstline = true;

			int counter = 0;
			std::cout << "L pressed" << std::endl;
			std::ifstream in_computed (ComputedSaliencyTxtPath);
			std::ifstream in_selected (UserSelectedSaliencyTxtPath);

			std::string line;
			std::string line_saliency_value;

			std::stringstream line_out;
			std::stringstream line_out_weighted;

			// temporary vectors to hold computed and user-selected saliency values
			std::vector <float> computed_saliency_values;
			std::vector <float> user_selected_saliency_values;

			// temporary vectors to hold pairs of unweighted and weighted differences
			std::vector <float> temp_diffs_unweighted;
			std::vector <float> temp_diffs_weighted;


		// step 1: get computed saliency values and store them in computed_saliency_values
			while (getline (in_computed,line)) {
				// ignore first line
				if (line.find("vertId") != std::string::npos) {
					continue;
				}
				line_saliency_value = line.substr(line.find("\t")+1, line.size());
				computed_saliency_values.push_back((float)std::stof(line_saliency_value));
			}

		// step 2: get user selected saliency values and store them in user_selected_saliency_values
			while (getline (in_selected, line)) {
				// ignore first line
				if (line.find("vertexId") != std::string::npos) {
					continue;
				}
				line_saliency_value = line.substr(line.find("\t")+1, line.size());
				user_selected_saliency_values.push_back((float)std::stof(line_saliency_value));
			}

		// step 3: calculate differences and store them in temp_diffs_unweighted
			for (int a=0; a!= computed_saliency_values.size(); a++) {
				float difference = computed_saliency_values[a] - user_selected_saliency_values[a];
				// store the (squared), absolute value of the difference in temp_diffs_weighted
//				temp_diffs_unweighted.push_back(pow(fabs(difference),2.0));
				temp_diffs_unweighted.push_back(fabs(difference));
			}

		// step 4: print unweighted differences to differencesPath
			std::ofstream out(differencesPath);
			std::string line_o = "";
			for (int b=0; b!=temp_diffs_unweighted.size(); b++) {
				line_o.append(std::to_string(verticesList[b].first));
				line_o.append("\t");
				line_o.append(std::to_string(temp_diffs_unweighted[b]));
				out << line_o << std::endl;
				line_o = "";
			}
			out.close();

		// step 5: iterate through unweighted difference values
		// where needed - compute weighted values and store everything in temp_diffs_weighted
		// print results to differences_weighted.txt
			std::ofstream out_weighted(differencesWeightedPath);
			int countWeighted = 0;
			for (int c=0; c!= temp_diffs_unweighted.size(); c++) {
				vertexSelection.clear();	// clear selection
				myOcTree->addVerticesToSelectionByCoordinates (verticesList[c].second, maxSearchRadius, vertexSelection, false);

				// compute average computed mesh saliency
				std::set<size_t>::iterator it = vertexSelection.begin();
				int number = 0;	// number of neighbor vertices that have a saliency value other than 0.0
				float average = 0.0;
				for (int n=0; n != vertexSelection.size(); ++n) {
					std::string line_out_dif_weighted = "";
					if (saliency_values[*it].second != 0.0) {
						++number;
						average += saliency_values[*it].second;
//						line.append("\t");
//						line.append(std::to_string(saliency_values[*it].second));
						std::advance(it, 1);
					}
				}
				std::string line_out_dif_weighted = "";
				line_out_dif_weighted.append(std::to_string(verticesList[c].first));
				line_out_dif_weighted.append("\t");

				if(number > 0) {
					if (average/number > threshold_weighted) {
						countWeighted++;
						float weighted_value = circleAtOriginRadiusOne(computed_saliency_values[c]);
						temp_diffs_weighted.push_back(weighted_value);
						line_out_dif_weighted.append(std::to_string(weighted_value));
					} else {
						temp_diffs_weighted.push_back(temp_diffs_unweighted[c]);
						line_out_dif_weighted.append(std::to_string(temp_diffs_unweighted[c]));
					}
				} else {
					temp_diffs_weighted.push_back(temp_diffs_unweighted[c]);
					line_out_dif_weighted.append(std::to_string(temp_diffs_unweighted[c]));
				}

				line_out_dif_weighted.append("\t");
				line_out_dif_weighted.append(std::to_string(vertexSelection.size()));
				out_weighted << line_out_dif_weighted << std::endl;
				line_out_dif_weighted = "";
			}
			out_weighted.close();

			std::cout << "number of weighted values: " << countWeighted << std::endl;

		// Step 6: compute overall difference and weighted difference
			float unweightedDifferenceOverall = 0.0;
			float weightedDifferenceOverall = 0.0;
			int lengthOverall = static_cast<int>(temp_diffs_unweighted.size());
			int countRelevantOnes = 0;

			for (int f=0; f<= temp_diffs_unweighted.size(); f++) {
				if (temp_diffs_unweighted[f] > threshold_difference) {
					countRelevantOnes++;
					unweightedDifferenceOverall += temp_diffs_unweighted[f];
					weightedDifferenceOverall += temp_diffs_weighted[f];
				} else {
					continue;
				}
			}

			std::cout << "number of differences > 0.001: " << countRelevantOnes << std::endl;
			std::cout << "OVERALL DIFFERENCE (unweighted): " << unweightedDifferenceOverall/countRelevantOnes << std::endl;
			std::cout << "OVERALL DIFFERENCE (weighted): " << weightedDifferenceOverall/countRelevantOnes << std::endl;

			break;
		}

		case 'c':
		case 'C':
		{
			// reset current selection to be empty
			float colors[4] = {0.89, 0.89, 0.89, 1.0};
			meshes[0]->changeColors(vertexSelection, colors);
			vertexSelection.clear();
			object1.updateAllMeshes();
			std::cout << "vertex selection cleared" << std::endl;
			break;
		}

		default:
			break;
	}
}

// MOUSE: pan
void motionFunction (int x, int y) {
	if (middle_button_state) {
		translation[0] += float(x - old_mouse_x) / float(max_window_width) * screen_width;
		translation[1] += -float(y - old_mouse_y) / float(max_window_height) * screen_height;
		old_mouse_x = x;
		old_mouse_y = y;
	}
	else if(left_button_state) {
		arcball.mouse_motion(x,y);
	}
	glutPostRedisplay();
}

// MOUSE: pan, rotate
void mouseFunction (int Button, int state, int x, int y) {
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

	if(left_button_state) {
		arcball.mouse_motion(x,y);
	}
	else if (Button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN){
			left_button_state = 1;
			arcball.mouse_down(x,y);
		} else{
			left_button_state = 0;
			arcball.mouse_up();
		}
	}
}

	// Simple function to read a shader file (from "https://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)" )
char* filetobuf(char *file)
{
    FILE *fptr;
    long length;
    char *buf;

    fptr = fopen(file, "rb"); /* Open file for reading */
    if (!fptr) /* Return NULL on failure */{
		std::cout<<"could not open "<<file<<std::endl;
        return NULL;
	}
    fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
    length = ftell(fptr); /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
    fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr); /* Close the file */
    buf[length] = 0; /* Null terminator */

    return buf; /* Return the buffer */
}

	// Simple function to create, compile and link shaders (from "https://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)" )
bool createShaders(){
	int IsCompiled_VS, IsCompiled_FS;
    int IsLinked;
    int maxLength;
    char *vertexInfoLog;
    char *fragmentInfoLog;
    char *shaderProgramInfoLog;

	/* These pointers will receive the contents of our shader source code files */
    char *vertexsource, *fragmentsource;

    /* These are handles used to reference the shaders */
    GLuint vertexshader, fragmentshader;

	 /* Read our shaders into the appropriate buffers */

/* ################### - 4.10 - (REPLACE FOLLOWING TWO LINES) ################### */
    vertexsource = filetobuf("/home/ri43hiq/opengl_tutorial/shader/vertex.vert");
    fragmentsource = filetobuf("/home/ri43hiq/opengl_tutorial/shader/fragment.frag");

    vertexshader = glCreateShader(GL_VERTEX_SHADER); // Create an empty vertex shader handle

    /* Send the vertex shader source code to GL */
    /* Note that the source code is NULL character terminated. */
    /* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
    glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);

    /* Compile the vertex shader */
    glCompileShader(vertexshader);

    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
    if(IsCompiled_VS == FALSE)
    {
       glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);

       /* The maxLength includes the NULL character */
       vertexInfoLog = (char *)malloc(maxLength);

       glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);
	   	   	 std::cout<<"could not compile vertex shader: \n"<<vertexInfoLog<<std::endl;
       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
       /* In this simple program, we'll just leave */
       free(vertexInfoLog);
	   free(vertexsource);
	   free(fragmentsource);
       return false;
    }

    /* Create an empty fragment shader handle */
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

    /* Send the fragment shader source code to GL */
    /* Note that the source code is NULL character terminated. */
    /* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
    glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);

    /* Compile the fragment shader */
    glCompileShader(fragmentshader);

    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
    if(IsCompiled_FS == FALSE)
    {
       glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);

       /* The maxLength includes the NULL character */
       fragmentInfoLog = (char *)malloc(maxLength);

       glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);
	   	 std::cout<<"could not compile fragment shader: \n"<<fragmentInfoLog<<std::endl;
       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
       /* In this simple program, we'll just leave */
       free(fragmentInfoLog);
	   free(vertexsource);
	   free(fragmentsource);
       return false;
    }

    /* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
    /* We must link them together to make a GL shader program */
    /* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
    /* Assign our program handle a "name" */
    shaderprogram = glCreateProgram();

    /* Attach our shaders to our program */
    glAttachShader(shaderprogram, vertexshader);
    glAttachShader(shaderprogram, fragmentshader);


	/* ################### - 4.11 - ################### */


    /* Link our program */
    /* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
    /* The binary code is uploaded to the GPU, if there is no error. */
    glLinkProgram(shaderprogram);

    /* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
    /* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
    /* too many texel fetch instructions or too many interpolators or dynamic loops. */

    glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int *)&IsLinked);
    if(IsLinked == FALSE)
    {
       /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
       glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);

       /* The maxLength includes the NULL character */
       shaderProgramInfoLog = (char *)malloc(maxLength);

       /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
       glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);
	   std::cout<<"could not link program: \n"<<shaderProgramInfoLog<<std::endl;

       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
       /* In this simple program, we'll just leave */
       free(shaderProgramInfoLog);
	   free(vertexsource);
	   free(fragmentsource);
       return false;
    }

	return true;
}

FIBITMAP* loadImage(const std::string filename){
    FreeImage_Initialise();

    // Determine the format of the image.
    // Note: The second paramter ('size') is currently unused, and we should use 0 for it.
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str(), 0);

    // Image not found? Abort! Without this section we get a 0 by 0 image with 0 bits-per-pixel but we don't abort, which
    // you might find preferable to dumping the user back to the desktop.
    if (format == -1) {
        std::cerr << "Could not find image: " << filename << " - Aborting." << std::endl;
        return false;
    }

    // Found image, but couldn't determine the file format? Try again...
    if (format == FIF_UNKNOWN) {
        std::cerr << "Couldn't determine file format - attempting to get from file extension..." << std::endl;

        // ...by getting the filetype from the filename extension (i.e. .PNG, .GIF etc.)
        // Note: This is slower and more error-prone that getting it from the file itself,
        // also, we can't use the 'U' (unicode) variant of this method as that's Windows only.
        format = FreeImage_GetFIFFromFilename(filename.c_str());

        // Check that the plugin has reading capabilities for this format (if it's FIF_UNKNOWN,
        // for example, then it won't have) - if we can't read the file, then we bail out =(
        if (!FreeImage_FIFSupportsReading(format)) {
            std::cerr << "Detected image format cannot be read!" << std::endl;
            return false;
        }
    }

    // If we're here we have a known image format, so load the image into a bitap
    FIBITMAP* bitmap = FreeImage_Load(format, filename.c_str());

    // How many bits-per-pixel is the source image?
    int bitsPerPixel = FreeImage_GetBPP(bitmap);
    FIBITMAP* bitmap32;
    if (bitsPerPixel == 32) {
            bitmap32 = bitmap;
    } else {

        bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
        FreeImage_Unload(bitmap);
    }


    return bitmap32;
}

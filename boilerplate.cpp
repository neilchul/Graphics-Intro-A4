// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Co-Authors:
//			Jeremy Hart, University of Calgary
//			John Hall, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imagebuffer.h"
#include "parser.h"

#include <math.h>

#define PI 3.14
using namespace std;
using namespace glm;
// --------------------
const vec3 origin = vec3(0,0,0);
const float delimitor = 9999.f;


// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);


// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
GLuint InitializeShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// check for OpenGL errors and return false if error occurred
	return program;
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry){

	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);

	//Set up Vertex Array Object
	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(
		VERTEX_INDEX,		//Attribute index 
		2, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec2),		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(VERTEX_INDEX);

	// associate the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(
		COLOUR_INDEX,		//Attribute index 
		3, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec3), 		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(Geometry *geometry, vec2 *vertices, vec3 *colours, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, colours, GL_STATIC_DRAW);

	//Unbind buffer to reset to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(Geometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(Geometry *geometry, GLuint program)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(GL_POINTS, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}


vector<vec3> generateRay(float viewAngle, float displaySizeX, float displaySizeY){
	
	vector<vec3> rays;
	
	viewAngle *= (PI/180.f);

	//finding the z value and setting up the base vector 
	float z = displaySizeX/(2* tan(viewAngle/2));
	vec3 tl = vec3 ((-displaySizeX/2)+0.5f, 
					(-displaySizeY/2)+0.5f,
					-z
				);


	//calculate for the rays the goes through each pixel
	for (int i = 0; i < displaySizeX;i++){
		for (int j = 0; j< displaySizeY; j++){
			vec3 rayDirection =  tl + vec3(i,j,0);
			rayDirection = normalize(rayDirection);
			rays.push_back(rayDirection);
		}
	}


	return rays;
}

/*
triangle contains a, b, and c where u = b-a and v = c-a 
returns r,s,t value for 
		a + ru + sv = p + dt
in that order 
*/
vec3 intersectTriangle(vec3 d, vec3 p, triangle t){
	vec3 u = t.b-t.a;
	vec3 v = t.c-t.a;
	
    mat3 duv = mat3(d, -u, -v);
    duv = inverse(duv);

    vec3 ap = t.a - p;

    return duv*ap;  
}

float intersectSphere(vec3 direction, vec3 center, vec3 oPoint, float radius){
	
	float a = dot(direction, direction);
	float b = 2* dot(oPoint - center, direction);
	float c = dot (oPoint - center, oPoint - center);
	c -= pow(radius,2);

	if (pow(b,2) - (4*a*c) < 0 || a == 0)
		return delimitor;

	float t = (-b - sqrt(pow(b,2) - (4*a*c))) / (2*a);

	return t;
}

float intersectPlane(vec3 d, vec3 q, vec3 oPoint, vec3 n){
	
	//if (dot(d,n) > 0)
	//	return delimitor;

	float t = dot(q-oPoint, n);
	t /= dot(d,n);

	return t;

}


bool shadow(vec3 dt, parser p){
	lightSource light = p.lightSources[0];
	float t = delimitor;
	float x = light.pos.x - dt.x, y = light.pos.y - dt.y, z = light.pos.z - dt.z;
	float maxT = sqrt(pow(x,2)+pow(y,2)+pow(z,2));

	
	vec3 ray = normalize(light.pos - dt);

	for (triangle tri : p.triangles){	
		vec3 v = intersectTriangle(ray, dt ,tri);
		if (v[0] < t && v[0] > 0 && v[1] >= 0 && v[2] >= 0 && (v[1]+v[2] <= 1)){
			t = v[0];
			if(t < maxT)
				return true;
		}
				
	}

	for (sphere sph : p.spheres){
		float f = intersectSphere(ray, sph.center, dt, sph.radius);
		if (f < t && f > 0){
			t = f;
			if(t < maxT)
				return true;
		}
			
	}

	for (plane pln : p.planes){
		float f = intersectPlane(ray, pln.q, dt, pln.n);
		if (f < t && f > 0){
			t = f;
			if(t < maxT)
				return true;
		}	
	}

	return false;
}



 /*
	check in a ray intersects with anything in the scene
	return the color at the intersection
	if there is no intersection, default to black 
 */
vec3  intersect(vec3 ray, parser p, vec3 oPoint){

	float t = delimitor;
	vec3 resultColor = vec3(0,0,0);
	lightSource light = p.lightSources[0];

	for (triangle tri : p.triangles){
		vec3 v = intersectTriangle(ray, oPoint ,tri);
		if (v[0] < t && v[0] > 0 && v[1] >= 0 && v[2] >= 0 && (v[1]+v[2] <= 1)){
			
			t = v[0];

			vec3 l = normalize(light.pos - (ray*t));
			vec3 h = -ray + l ;
			h = h/ ((float)h.length());
			h = normalize(h);
			vec3 normal = normalize(cross(tri.b-tri.a, tri.c-tri.a));

			float max = dot(normal, l);
			if (max < 0)
				max = 0;


			if(!shadow(ray*t + (normal * 0.0001f), p))
				resultColor = (tri.Cr * (light.Ca + (light.Cl*  max ))) 
							+ (light.Cl * tri.Cp * pow(dot(h, normal),tri.phong));
			else
				resultColor = tri.Cr * light.Ca;
			
		}
			
	}

	for (sphere sph : p.spheres){
		float f = intersectSphere(ray, sph.center, oPoint, sph.radius);
		if (f < t && f > 0){
			t = f; 

			vec3 l = normalize(light.pos - (ray*t));
			vec3 h = -ray + l ;
			h = h/ ((float)h.length());
			h = normalize(h);
			vec3 normal = normalize(  (ray*t) - sph.center);
			
			float max = dot(normal, l);
			if (max < 0)
				max = 0;


			//reflection
			vec3 reflectedRay = ray - (2*(dot(ray, normal))*normal);

			//angle between d and l / 2 = angle between n and l 
			//vec3 temp = d*l / 2.f;
			//printf("sph: (%f,%f,%f) || (%f, %f, %f) \n", sph.Cr.x, sph.Cr.y, sph.Cr.y, sph.Cp.x, sph.Cp.y, sph.Cp.z);
			if(!shadow(ray*t + (normal * 0.0001f), p))
				resultColor = (sph.Cr * (light.Ca + (light.Cl*  max ))) 
							+ (light.Cl * sph.Cp * pow(dot(h, normal),sph.phong));
			else 
				resultColor = sph.Cr * light.Ca;
			
			resultColor *= intersect(reflectedRay, p, ray*t +(normal * 0.0001f));
		}	
	}

	for (plane pln : p.planes){
		float f = intersectPlane(ray, pln.q, oPoint, pln.n);
		if (f < t && f > 0){
			t = f;

			vec3 l = normalize(light.pos - (ray*t));
			vec3 h = -ray + l ;
			h = h/ ((float)h.length());
			h = normalize(h);
			vec3 normal = normalize(pln.n);
			
			float max = dot(normal, l);
			if (max < 0)
				max = 0;

			if(!shadow(ray*t +(normal * 0.0001f), p))
				resultColor = (pln.Cr * (light.Ca + (light.Cl*  max ))) 
							+ (light.Cl * pln.Cp * pow(dot(h, normal),pln.phong));
			else
				resultColor = pln.Cr*light.Ca;
			
		
		}
	}


	return resultColor;

}






void generateScene(ImageBuffer &iBuff, parser p, vector<vec3> rays,  int wnd_width, int wnd_height){

	int index = 0;
	for (int w = 0; w < wnd_width; w++){
		for(int h = 0; h < wnd_height; h++){
			//vec3 v = vec3(1,1,1)*(float)(h/(512.f));
			iBuff.SetPixel(w,h, intersect(rays[index], p, origin));
			index++;
		}
	}

	

}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 512, height = 512;
	window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	//Intialize GLAD
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	GLuint program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// three vertex positions and assocated colours of a triangle
	vector <vec2> vertices;

/*
	for (int x = -1; x < 1; x+= 0.01){
		for (int y = 1; y >-1; y-=0.01){
			vertices.push_back(vec2(x,y));
		}
	}
*/


	vector <vec3> rays = generateRay(55.f, 512.f, 512.f); 
	ImageBuffer iBuff1; 
	iBuff1.Initialize();	
	parser scene1;
	scene1.extractShapes("scenes/scene1.txt");
	
	/*
	int index = 0;
	float t = 10000.f;
	for (int w = 0; w < width; w++){
		for(int h = 0; h < height; h++){
			//vec3 v = vec3(1,1,1)*(float)(h/(512.f));
			iBuff1.SetPixel(w,h, intersect(rays[index], scene1, t));
			cout << t <<endl; 
			index++;
		}
	}
	*/

	generateScene(iBuff1, scene1, rays,  width, height);
	//iBuff1 = generateScene(iBuff1, scene1, rays, width, height);
	


	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		// call function to draw our scene
		iBuff1.Render();
		


		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	//DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}

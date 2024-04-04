#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <array>

#include "triangle.h"
#include "helper.h"         
#include "objGen.h" 
#include "trackball.h"

#include "particle.cpp"
#include "constraint.cpp"
#include "springPhysics.h"
#include "objects.h"

#include <chrono>
#include <ctime>

#pragma warning(disable : 4996)
#pragma comment(lib, "glfw3.lib")


using namespace std;

TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;

vector <TriangleC> tri;
std::string filename = "geometry.obj";

GLuint points = 0;
int pointSize = 1;
int lineWidth = 1;
int squares = 30; // controls cloth resolution
GLdouble mouseX, mouseY;
GLfloat deltaT = 0.0001f;

//Vertex array object and vertex buffer object indices 
GLuint VAO, VBO;

extern bool friction; // For determining how the cloth collides with spheres
bool drawSpheres = true; // Whether to draw spheres or just leave them as invisible boundaries

// Used for the dropdown to determine how the cloth is made
const char* lockModes[] = { "No corners locked", "2 corners locked", "4 corners locked" };
const char* currentLockMode = lockModes[0];
int lock = 0;

extern bool enableMinMaxStretch;

extern GLfloat defaultK; // determines stretchiness of constraints


inline void AddVertex(vector <GLfloat>* a, glm::vec3 A)
{
	a->push_back(A[0]); a->push_back(A[1]); a->push_back(A[2]);
}

inline void AddParticle(vector <Particle>* a, Particle P)
{
	a->push_back(P);
}

inline void AddConstraint(vector <Constraint>* a, Constraint C)
{
	a->push_back(C);
}

//Set up particles and constraints
void CreateParticles(vector <Particle>* a, int squares)
{
	Particle p;
	glm::vec3 v;

	GLfloat r = 2.0f;
	GLfloat h = 1.5f;

	GLfloat deltaX = r / (GLfloat)squares;
	GLfloat deltaZ = r / (GLfloat)squares;

	for (GLuint i = 0; i < squares; i++)
	{

		GLfloat z = (i * deltaZ) - (r / 2.f);

		for (GLuint j = 0; j < squares; j++)
		{

			GLfloat x = (j * deltaX) - (r / 2.f);

			v = glm::vec3(r * x,
				h,
				r * z);

			if (lock == 0) {
				AddParticle(a, Particle(v, false));
			}
			else {
				// lock two of the corners
				if ((i == 0) && ((j == 0) || (j % squares == squares - 1)) && (lock != 0)) {
					AddParticle(a, Particle(v, true));
				}
				else if ((i == squares - 1) && ((j == 0) || (j % squares == squares - 1)) && (lock == 2)) {
					AddParticle(a, Particle(v, true));
				}
				else {
					AddParticle(a, Particle(v, false));
				}
			}	
		}
	}
}

void CreateConstraints(vector <Particle> *p, vector <Constraint> *c) {
	Particle* p1;
	Particle* p2;
	Constraint x;

	for (int i = 0; i < p->size(); i++) {
		// connect top right neighbor
		if (((i % squares) < (squares - 1)) && ((i / squares) > 0)) {
			p1 = &p->at(i);
			p2 = &p->at(i + 1 - squares);
			x = Constraint(p1, p2);
			p1->AddConstraint(x); p2->AddConstraint(x); AddConstraint(c, x);
		}
		// connect right neighbor
		if ((i % squares) < (squares - 1)) {
			p1 = &p->at(i);
			p2 = &p->at(i + 1);
			x = Constraint(p1, p2);
			p1->AddConstraint(x); p2->AddConstraint(x); AddConstraint(c, x);
		}
		// connect lower right neighbor
		if (((i % squares) < (squares - 1)) && ((i / squares) < (squares - 1))) {
			p1 = &p->at(i);
			p2 = &p->at(i + 1 + squares);
			x = Constraint(p1, p2);
			p1->AddConstraint(x); p2->AddConstraint(x); AddConstraint(c, x);
		}
		// connect lower neighbor
		if ((i / squares) < (squares - 1)) {
			p1 = &p->at(i);
			p2 = &p->at(i + squares);
			x = Constraint(p1, p2);
			p1->AddConstraint(x); p2->AddConstraint(x); AddConstraint(c, x);
		}
	}
}

void CreateCloth(vector <GLfloat>* a, vector <Particle> p) {

	for (int i = 0; i < p.size() - (squares + 1); i++)
	{
		if (i % squares == squares - 1) {
			continue;
		}
		//the first triangle
		AddVertex(a, p[i].pos);
		AddVertex(a, p[i + 1].pos);
		AddVertex(a, p[i + squares].pos);
		//the second triangle
		AddVertex(a, p[i+1].pos);
		AddVertex(a, p[i+squares].pos);
		AddVertex(a, p[i+squares+1].pos);

	}
}

void AddSphere(glm::vec3 pos, GLfloat r, vector<Sphere>* spheres) {
	spheres->push_back(Sphere(pos, r));
}

// Draw a sphere at the designated location, then add a sphere to the list of spheres
void CreateSphere(vector <GLfloat>* a, Sphere sphere)
{
	glm::vec3 v;

	GLfloat r = sphere.r;
	GLfloat x = sphere.pos[0];
	GLfloat y = sphere.pos[1];
	GLfloat z = sphere.pos[2];

	int s = (int) fabsf(r * 15.f) + 1;

	GLfloat deltaX = 1 / (GLfloat)s;
	GLfloat deltaZ = 1 / (GLfloat)s;

	GLfloat deltaTheta = 2 * M_PI / (GLfloat)s;
	GLfloat deltaPhi = M_PI / (GLfloat)s;

	for (GLuint i = 0; i < s; i++)
	{
		GLfloat phi = i * deltaPhi;

		for (GLuint j = 0; j < s; j++)
		{
			GLfloat theta = j * deltaTheta;

			//the first triangle
			v = glm::vec3(x + r * cos(theta) * sin(phi),
				y + r * sin(theta) * sin(phi),
				z + r * cos(phi));
			AddVertex(a, v);
			v = glm::vec3(x + r * cos(theta + deltaTheta) * sin(phi),
				y + r * sin(theta + deltaTheta) * sin(phi),
				z + r * cos(phi));
			AddVertex(a, v);
			v = glm::vec3(x + r * cos(theta + deltaTheta) * sin(phi + deltaPhi),
				y + r * sin(theta + deltaTheta) * sin(phi + deltaPhi),
				z + r * cos(phi + deltaPhi));
			AddVertex(a, v);
			//the second triangle
			v = glm::vec3(x + r * cos(theta) * sin(phi),
				y + r * sin(theta) * sin(phi),
				z + r * cos(phi));
			AddVertex(a, v);
			v = glm::vec3(x + r * cos(theta + deltaTheta) * sin(phi + deltaPhi),
				y + r * sin(theta + deltaTheta) * sin(phi + deltaPhi),
				z + r * cos(phi + deltaPhi));
			AddVertex(a, v);
			v = glm::vec3(x + r * cos(theta) * sin(phi + deltaPhi),
				y + r * sin(theta) * sin(phi + deltaPhi),
				z + r * cos(phi + deltaPhi));
			AddVertex(a, v);
		}
	}
}


int CompileShaders() {
	//Vertex Shader
	const char* vsSrc= "#version 330 core\n"
		"layout (location = 0) in vec4 iPos;\n"
		"uniform mat4 modelview;\n"
		"void main()\n"
		"{\n"
		"   vec4 oPos=modelview*iPos;\n"
		"   gl_Position = vec4(oPos.x, oPos.y, oPos.z, oPos.w);\n"
		"}\0";

	//Fragment Shader
	const char* fsSrc = "#version 330 core\n"
		"out vec4 col;\n"
		"uniform vec4 color;\n"
		"void main()\n"
		"{\n"
		"   col = color;\n"
		"}\n\0";

	//Create VS object
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	//Attach VS src to the Vertex Shader Object
	glShaderSource(vs, 1, &vsSrc, NULL);
	//Compile the vs
	glCompileShader(vs);

	//The same for FS
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSrc, NULL);
	glCompileShader(fs);

	//Get shader program object
	GLuint shaderProg = glCreateProgram();
	//Attach both vs and fs
	glAttachShader(shaderProg, vs);
	glAttachShader(shaderProg, fs);
	//Link all
	glLinkProgram(shaderProg);

	//Clear the VS and FS objects
	glDeleteShader(vs);
	glDeleteShader(fs);
	return shaderProg;
}

void BuildScene(GLuint& VBO, GLuint& VAO, int squares, vector<Particle> particles, vector<Sphere> spheres) { //return VBO and VAO values n is the subdivision
	vector<GLfloat> v;
	
	if (drawSpheres) {
		for (int i = 0; i < spheres.size(); i++) {
			Sphere s = spheres.at(i);
			CreateSphere(&v, s);
		}
	}

	CreateCloth(&v, particles);
	//now get it ready for saving as OBJ
	tri.clear();
	for (unsigned int i = 0; i < v.size(); i += 9) { //stride 3 - 3 vertices per triangle
		TriangleC tmp;
		glm::vec3 a, b, c;
		a=glm::vec3(v[i], v[i + 1], v[i + 2]);
		b=glm::vec3(v[i + 3], v[i + 4], v[i + 5]);
		c = glm::vec3(v[i + 6], v[i + 7], v[i + 8]);
		tmp.Set(a, b, c); //store them for 3D export
		tri.push_back(tmp);
	}

	//make VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//bind it
	glBindVertexArray(VAO);

	//bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//send the data to the GPU
	points = v.size();
	glBufferData(GL_ARRAY_BUFFER, points * sizeof(GLfloat), &v[0], GL_STATIC_DRAW);

	//Configure the attributes
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//Make it valid
	glEnableVertexAttribArray(0);

	v.clear(); // no need for the data, it is on the GPU now

}

//Quit when ESC is released
static void KbdCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//set the callbacks for the virtual trackball
//this is executed when the mouse is moving
void MouseCallback(GLFWwindow* window, double x, double y) {
	//do not forget to pass the events to ImGUI!
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y);
	if (io.WantCaptureMouse) return; //make sure you do not call this callback when over a menu
//now process them
	mouseX = x;
	mouseY = y;
	//we need to perform an action only if a button is pressed
	if (mouseLeft)  trackball.Rotate(mouseX, mouseY); 
	if (mouseMid)   trackball.Translate(mouseX, mouseY);
	if (mouseRight) trackball.Zoom(mouseX, mouseY);
}


//set the variables when the button is pressed or released
void MouseButtonCallback(GLFWwindow* window, int button, int state, int mods) {
//do not forget to pass the events to ImGUI!
	
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, state);
	if (io.WantCaptureMouse) return; //make sure you do not call this callback when over a menu

//process them
	if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS)
	{
		trackball.Set(window, true, mouseX, mouseY);
		mouseLeft = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT  && state == GLFW_RELEASE)
	{
		trackball.Set(window, false, mouseX, mouseY);
		mouseLeft = false;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_PRESS)
	{
		trackball.Set(window, true, mouseX, mouseY);
		mouseMid = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_RELEASE)
	{
		trackball.Set(window, true, mouseX, mouseY);
		mouseMid = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS)
	{
		trackball.Set(window, true, mouseX, mouseY);
		mouseRight = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_RELEASE)
	{
		trackball.Set(window, true, mouseX, mouseY);
		mouseRight = false;
	}
}

void ResetScene(GLuint& VBO, GLuint& VAO, int squares, vector<Particle> *particles, vector<Constraint> *constraints, vector<Sphere> *spheres) {
	particles->clear();
	constraints->clear();
	CreateParticles(particles, squares);
	CreateConstraints(particles, constraints);

	BuildScene(VBO, VAO, squares, *particles, *spheres); //rebuild scene if the subdivision has changed
}

int main()
{
	glfwInit();

	//negotiate with the OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//make OpenGL window
	GLFWwindow* window = glfwCreateWindow(800, 800, "Cloth Sim", NULL, NULL);
	//is all OK?
	if (window == NULL)
	{
		std::cout << "Cannot open GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Paste the window to the current context
	glfwMakeContextCurrent(window);

	//Load GLAD to configure OpenGL
	gladLoadGL();
	//Set the viewport
	glViewport(0, 0, 800, 800);

	// once the OpenGL context is done, build the scene and compile shaders
	// the cloth
	vector <Particle> particles;
	vector <Constraint> constraints;
	CreateParticles(&particles, squares);
	CreateConstraints(&particles, &constraints);

	// other objects for the cloth to collide with
	vector <Sphere> spheres;
	AddSphere(glm::vec3(0.f, 0.f, 0.f), 1.f, &spheres);

	BuildScene(VBO, VAO, squares, particles, spheres);
	int shaderProg = CompileShaders();
	GLint modelviewParameter = glGetUniformLocation(shaderProg, "modelview");

	//Background color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Use shader
	glUseProgram(shaderProg);
	glPointSize(pointSize);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	bool drawScene = true;
	bool filled = false;
	float color[4] = { 0.8f, 0.8f, 0.2f, 1.0f };
	//send the color to the fragment shader
	glUniform4f(glGetUniformLocation(shaderProg, "color"), color[0], color[1], color[2], color[3]);

	glfwSetKeyCallback(window, KbdCallback); //set keyboard callback to quit
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//Clean the window
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGUI window creation
		ImGui::Begin("Cloth Sim");
		//checkbox to render or not the scene
		ImGui::Checkbox("Draw Scene", &drawScene);
		//checkbox to render or not the scene
		if (ImGui::Checkbox("Filled", &filled)) {
			if (filled) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		ImGui::Checkbox("Draw Spheres", &drawSpheres);
		/*
		if (ImGui::Checkbox("Friction", &friction)) {
			ResetScene(VBO, VAO, squares, &particles, &constraints, &spheres);
		}
		*/
		if (ImGui::Checkbox("Enable Min-Max Stretch", &enableMinMaxStretch)) {
			ResetScene(VBO, VAO, squares, &particles, &constraints, &spheres);
		}
		/*
		if (ImGui::Button("Save OBJ")) {
			SaveOBJ(&tri, filename);
			//ImGui::OpenPopup("Saved");
		}
		*/
		if (ImGui::BeginCombo("Locked Corners", currentLockMode)) {
			for (int i = 0; i < 3; i++) {
				bool isSelected = (currentLockMode == lockModes[i]);
				if (ImGui::Selectable(lockModes[i], isSelected)) {
					currentLockMode = lockModes[i];
					lock = i;
					ResetScene(VBO, VAO, squares, &particles, &constraints, &spheres);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::SliderInt("Cloth Resolution", &squares, 2, 100, "%d", 0)) {
			ResetScene(VBO, VAO, squares, &particles, &constraints, &spheres);
		}
		if (ImGui::SliderInt("Point Size", &pointSize, 1, 10, "%d", 0)) {
			glPointSize(pointSize); //set the new point size if it has been changed			
		}
		if (ImGui::SliderInt("Line Width", &lineWidth, 1, 10, "%d", 0)) {
			glLineWidth(lineWidth); //set the new point size if it has been changed			
		}
		if (ImGui::ColorEdit4("Color", color)) { //set the new color only if it has changed
			glUniform4f(glGetUniformLocation(shaderProg, "color"), color[0], color[1], color[2], color[3]);
		}
		ImGui::SliderFloat("Time Step", &deltaT, 0.0f, 0.001f, "%.5f");
		if (ImGui::SliderFloat("Constraint Elasticity", &defaultK, 1.f, 100.f, "%.1f")) {
			ResetScene(VBO, VAO, squares, &particles, &constraints, &spheres);
		}

		// Ends the window
		ImGui::End();

		// Perform a step of physics simulation and rebuild scene
		vector <Particle> newParticles = particles;
		for (int i = 0; i < particles.size(); i++) {
			spring::updateParticle(&particles[i], &newParticles[i], spheres, deltaT);
		}

		particles = newParticles;
		
		BuildScene(VBO, VAO, squares, particles, spheres);
		// Done with physics step

		//set the projection matrix
		glm::mat4 proj = glm::perspective(65.f, 1.f, 0.01f, 1000.f);
		//set the viewing matrix (looking from [0,0,5] to [0,0,0])
		glm::mat4 view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//get the modeling matrix from the trackball
		glm::mat4 model = trackball.Set3DViewCameraMatrix();
		//premultiply the modelViewProjection matrix
		glm::mat4 modelView = proj * view * model;
		//and send it to the vertex shader
		glUniformMatrix4fv(modelviewParameter, 1, GL_FALSE, glm::value_ptr(modelView));
		
		if (drawScene) {
			glDrawArrays(GL_POINTS,    0, points / 3);
			glDrawArrays(GL_TRIANGLES, 0, points / 3);
		}

		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//make sure events are served
		glfwPollEvents();
	}
	//Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProg);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

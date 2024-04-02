#ifndef PARTICLE_H
#define PARTICLE_H

#include <memory.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <vector>

#include "constraint.h"

using namespace std;

// Particle class that defines each particle, their position, and the constraints between them.

class Particle {

public:

	// variables
	bool locked; // whether the particle is able to move
	glm::vec3 pos; // position
	glm::vec3 v = glm::vec3(0.f, 0.f, 0.f); // velocity
	glm::vec3 a = glm::vec3(0.f, 0.f, 0.f); // acceleration
	GLfloat m = 0.001f; // mass
	vector <Constraint> cons;

	// constructors
	Particle(void);

	Particle(glm::vec3 _pos);

	Particle(glm::vec3 _pos, bool _locked);

	Particle(glm::vec3 _pos, GLfloat _m);

	Particle(glm::vec3 _pos, GLfloat _m, bool _locked);

	// destructor
	~Particle(void);

	// functions
	void AddConstraint(Particle *p, vector<Constraint>* c);
	void AddConstraint(Constraint c);

};

#endif
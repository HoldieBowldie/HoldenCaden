#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <memory.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Particle;

class Constraint {
	
public:

	// constructors
	Constraint(void) {
		a = glm::vec3();
		b = glm::vec3();
		k = 1.f;
		length = 1.f;
	}

	Constraint(glm::vec3 _a, glm::vec3 _b, GLfloat _k) : a(_a), b(_b), k(_k) {
		a = _a;
		b = _b;
		k = _k;
		length = glm::length(a - b);
	}

	Constraint(glm::vec3 _a, glm::vec3 _b) {
		a = _a;
		b = _b;
		k = 1.f;
		length = glm::length(a - b);
	}

	// destructor
	~Constraint(void) {} // empty for now !!

	// variables
	glm::vec3 a; // first endpoint, pointer to a particle
	glm::vec3 b; // second endpoint
	GLfloat k; // elasticity
	GLfloat length; // default length of the constraint 

};

#endif
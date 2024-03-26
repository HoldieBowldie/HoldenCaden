#pragma once

#include "particle.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"


class Constraint {
	
public:
	// constructors
	Constraint(void) {}

	Constraint(Particle* _a, Particle* _b, GLfloat _k, GLfloat _length) {
		a = _a;
		b = _b;
		k = _k;
		length = _length;
	}

	// destructor
	~Constraint(void) {} // empty for now !!

	// variables
	Particle* a; // first endpoint, pointer to a particle
	Particle* b; // second endpoint, ^ 
	GLfloat k; // elasticity
	GLfloat length; // default length of the constraint 

};
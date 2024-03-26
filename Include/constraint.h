#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <memory.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Particle;

// Constraints between particles, their lengths, and their elasticity

class Constraint {

public:

	// constructors
	Constraint(void);

	Constraint(Particle* _a, Particle* _b, GLfloat _k);

	Constraint(Particle* _a, Particle* _b);

	// destructor
	~Constraint(void);

	// variables
	Particle* a; // first endpoint, pointer to a particle
	Particle* b; // second endpoint
	GLfloat k; // elasticity
	GLfloat restLength; // resting length of the constraint

	GLfloat length(void);

};

#endif
#pragma once

#include "particle.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"


class Constraint {
	
public:
	// constructors
	Constraint(void) {}

	// w/ length parameter, just in case we ever want the default length to be different?
	Constraint(Particle* _a, Particle* _b, GLfloat _k, GLfloat _length) {
		a = _a;
		b = _b;
		k = _k;
		length = _length;
	}

	// calculates length from position of two particles
	Constraint(Particle* _a, Particle* _b, GLfloat _k) {
		a = _a;
		b = _b;
		k = _k;
		length = a->distanceFrom(b);
	}

	// destructor
	~Constraint(void) {}

	// variables
	Particle* a; // first endpoint, pointer to a particle
	Particle* b; // second endpoint, ^ 
	GLfloat k; // elasticity
	GLfloat length; // default length of the constraint 

};

// form a constraint between two particles and add it to both of their constraint lists
void connectParticles(Particle* a, Particle* b, GLfloat k) {
	Constraint* c = new Constraint(a, b, k);
	a->constraints->push_back(c);
	b->constraints->push_back(c);
}
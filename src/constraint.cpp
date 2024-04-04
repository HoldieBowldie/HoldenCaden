#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "constraint.h"
#include "particle.h"

// Constraints between particles, their lengths, and their elasticity

// constructors
Constraint::Constraint(void) {
	a = NULL;
	b = NULL;
	k = 25.f;
	restLength = 0.f;
}

Constraint::Constraint(Particle* _a, Particle* _b, GLfloat _k) : a(_a), b(_b), k(_k) {
	a = _a;
	b = _b;
	k = _k;
	restLength = glm::length(a->pos - b->pos);
}

Constraint::Constraint(Particle* _a, Particle* _b) : a(_a), b(_b) {
	a = _a;
	b = _b;
	k = 15.0f;
	restLength = glm::length(a->pos - b->pos);
}

// destructor
Constraint::~Constraint(void) {} // empty for now !!

// calculate the current length of the constraint using its vertices
GLfloat Constraint::length(void) {
	return glm::length(a->pos - b->pos);
}
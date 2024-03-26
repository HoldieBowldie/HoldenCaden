#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "constraint.h"
#include "particle.h"

// Constraints between particles, their lengths, and their elasticity

// constructors
Constraint::Constraint(void) {
	a = NULL;
	b = NULL;
	k = 1.f;
	length = 0.f;
}

Constraint::Constraint(Particle* _a, Particle* _b, GLfloat _k) : a(_a), b(_b), k(_k) {
	a = _a;
	b = _b;
	k = _k;
	length = glm::length(a->pos - b->pos);
}

Constraint::Constraint(Particle* _a, Particle* _b) : a(_a), b(_b) {
	a = _a;
	b = _b;
	k = 1.f;
	length = glm::length(a->pos - b->pos);
}

// destructor
Constraint::~Constraint(void) {} // empty for now !!

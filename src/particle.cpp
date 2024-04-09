#include <memory.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <vector>

#include "particle.h"
#include "constraint.h"

using namespace std;

Particle::Particle(void) {
	pos = glm::vec3();
	lastPos = pos;
	locked = false;
}

Particle::Particle(glm::vec3 _pos) {
	pos = _pos;
	lastPos = pos;
	locked = false;
}

Particle::Particle(glm::vec3 _pos, bool _locked) {
	pos = _pos;
	lastPos = pos;
	locked = _locked;
}

Particle::Particle(glm::vec3 _pos, GLfloat _m) {
	pos = _pos;
	lastPos = pos;
	m = _m;
	locked = false;
}

Particle::Particle(glm::vec3 _pos, GLfloat _m, bool _locked) {
	pos = _pos;
	lastPos = pos;
	m = _m;
	locked = _locked;
}

// destructor
Particle::~Particle(void) {} // empty for now !!

void Particle::AddConstraint(Particle *p, vector<Constraint> *c) {
	Constraint C(this, p, 100.0f);
	cons.push_back(C);
	c->push_back(C);
}

void Particle::AddConstraint(Constraint c) {
	cons.push_back(c);
}


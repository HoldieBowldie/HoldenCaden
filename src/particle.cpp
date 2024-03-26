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
	locked = false;
}

Particle::Particle(glm::vec3 _pos) {
	pos = _pos;
	locked = false;
}

Particle::Particle(glm::vec3 _pos, bool _locked) {
	pos = _pos;
	locked = _locked;
}

Particle::Particle(glm::vec3 _pos, GLfloat _m) {
	pos = _pos;
	m = _m;
	locked = false;
}

Particle::Particle(glm::vec3 _pos, GLfloat _m, bool _locked) {
	pos = _pos;
	m = _m;
	locked = _locked;
}

// destructor
Particle::~Particle(void) {} // empty for now !!

void Particle::AddConstraint(Particle p) {
	Constraint C(this, &p);
	cons.push_back(C);
}


#ifndef PARTICLE_H
#define PARTICLE_H

#include <memory.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

#include "constraint.h"

using namespace std;

class Particle {

public:

	// variables
	bool locked; // whether the particle is able to move
	glm::vec3 pos; // the particle's position
	vector <Constraint> cons;

	// constructors
	Particle(void) {
		pos = glm::vec3();
		locked = false;
	}

	Particle(glm::vec3 _pos) {
		pos = _pos;
		locked = false;
	}

	Particle(glm::vec3 _pos, bool _locked) {
		pos = _pos;
		locked = _locked;
	}

	// destructor
	~Particle(void) {} // empty for now !!

	void AddConstraint(Particle p) {
		Constraint C(pos, p.pos);
		cons.push_back(C);
	}

};

#endif
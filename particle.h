#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Particle {

public:
	// constructors
	Particle(void) {}

	Particle(glm::vec3 _pos) {
		pos = _pos;
	}

	Particle(glm::vec3 _pos, bool _locked) {
		pos = _pos;
		locked = _locked;
	}

	// destructor
	~Particle(void) {} // empty for now !!

	// variables
	bool locked = false; // whether the particle is able to move
	glm::vec3 pos; // the particle's position
	

};
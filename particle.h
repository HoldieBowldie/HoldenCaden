#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <vector>

#include "constraint.h"

class Particle {

public:
	// constructors
	Particle(void) {}

	// locked is false by default
	Particle(glm::vec3 _pos) {
		pos = _pos;
		constraints = new std::vector<Constraint*>;
	}

	// determine whether you want it to be locked
	Particle(glm::vec3 _pos, bool _locked) {
		pos = _pos;
		locked = _locked;
		constraints = new std::vector<Constraint*>;
	}

	// destructor
	~Particle(void) {
		delete constraints;
	}


	// variables
	bool locked = false; // whether the particle is able to move
	glm::vec3 pos; // the particle's position
	glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f); // the particle's velocity
	std::vector<Constraint*> *constraints; // list of constraints with other particles
	

	// functions
	// calculate distance from this particle to another particle b
	GLfloat distanceFrom(Particle* b) {
		GLfloat dist = 0.0;
		dist += std::powf((this->pos[0] - b->pos[0]), 2);
		dist += std::powf((this->pos[1] - b->pos[1]), 2);
		dist += std::powf((this->pos[2] - b->pos[2]), 2);
		dist = std::sqrtf(dist);
		return dist;
	}
};
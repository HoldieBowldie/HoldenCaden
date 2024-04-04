#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "particle.h";

class Sphere {

public:
	glm::vec3 pos; // position
	GLfloat r; // radius

	Sphere(void) {
		pos = glm::vec3(0.f, 0.f, 0.f);
		r = 1.0f;
	}

	Sphere(glm::vec3 _pos, GLfloat _r) {
		pos = _pos;
		r = _r;
	}

	int collidesWith(Particle *p) {
		if (glm::distance(p->pos, pos) < r) {
			return 1;
		}
		return 0;
	}
};
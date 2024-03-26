#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "particle.h"
#include "constraint.h"

#include <chrono>
#include <ctime>
#include <vector>

using namespace std;

namespace spring {
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f); // Gravity vector
	glm::vec3 wind = glm::vec3(0.0f, 0.0f, 0.0f); // wind vector, 0 by default

	GLfloat a = 1.0f; // air resistance constant

	// Update a particle's acceleration
	void updateAcceleration(Particle* p) {
		glm::vec3 F = glm::vec3(0.0f, 0.0f, 0.0f); // force acting upon the particle

		F += (p->m * g);
		F += wind;
		F += -a * (p->v * p->v);
		for (int i = 0; i < p->cons.size(); i++) {
			Constraint c = p->cons[i];

			F -= c.k * (c.length() - c.restLength);
		}

		p->a = F / p->m; // acceleration = force / mass
	}

	// Iterate through all particles to update them
	void updateParticles(vector<Particle> particles) {
		// TODO
	}
}

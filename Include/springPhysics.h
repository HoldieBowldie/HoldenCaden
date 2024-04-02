#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "particle.h"
#include "constraint.h"

#include <chrono>
#include <ctime>
#include <vector>

#include <iostream>

using namespace std;

namespace spring {

	GLfloat c1 = 0.67560359598f;
	GLfloat c2 = -0.17560359598f;
	GLfloat c3 = -0.17560359598f;
	GLfloat c4 = 0.67560359598f;
	GLfloat d1 = 1.35120719196f;
	GLfloat d2 = -1.70241438392f;
	GLfloat d3 = 1.35120719196f;

	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f); // Gravity vector
	glm::vec3 wind = glm::vec3(0.0f, 0.0f, 0.0f); // wind vector, 0 by default

	GLfloat air = .05f; // air resistance constant

	// Update a particle's position, velocity, and acceleration.
	void updateParticle(Particle* p, Particle* newP, GLfloat deltaTime) {
		if (p->locked) {
			return; // locked particles do not move
		}

		glm::vec3 tempPos = p->pos;

		p->pos = p->pos + (c1 * p->v * deltaTime); // x1

		for (int i = 0; i < 3; i++) {

			glm::vec3 F = glm::vec3(0.0f, 0.0f, 0.0f); // force acting upon the particle

			F += (p->m * g); // Add gravity
			F += wind; // Add wind
			F += -air * (p->v * p->v); // Add air resistance

			// Add spring forces

			for (int i = 0; i < p->cons.size(); i++) {
				Constraint c = p->cons[i];

				glm::vec3 dir = p->pos - c.b->pos;				
				
				if (dir == glm::vec3(0.0f, 0.0f, 0.0f)) {
					dir = p->pos - c.a->pos;
				}
				

				glm::normalize(dir);

				F -= dir * (c.k * (c.length() - c.restLength));
			}

			/*
			p->pos = p->pos + deltaTime * p->v;
			p->v = p->v + deltaTime * F / p->m;
			p->a = F / p->m;
			*/

			p->a = F / p->m;

			switch (i)
			{
				case 0:
					p->v = p->v + (d1 * p->a * deltaTime); // v1
					p->pos = p->pos + (c2 * p->v * deltaTime); // x2
				case 1:
					p->v = p->v + (d2 * p->a * deltaTime); // v2
					p->pos = p->pos + (c3 * p->v * deltaTime); // x3
				case 2:
					p->v = p->v + (d3 * p->a * deltaTime); // v3
					p->pos = p->pos + (c4 * p->v * deltaTime); // x4
			}
		}

		newP->pos = p->pos;
		newP->v = p->v;
		p->pos = tempPos;
		

		return;
	}
}

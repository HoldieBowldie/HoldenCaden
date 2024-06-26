#ifndef SPRINGPHYSICS_H
#define SPRINGPHYSICS_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "particle.h"
#include "constraint.h"
#include "objects.h";

#include <chrono>
#include <ctime>
#include <vector>

#include <iostream>

bool friction = false;
bool enableMinMaxStretch = true;

glm::vec3 wind = glm::vec3(0.0f, 0.0f, 0.0f);

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

	GLfloat air = .001f; // air resistance constant

	// Update a particle's position, velocity, and acceleration.
	void updateParticle(Particle* p, Particle* newP, vector<Sphere> spheres, GLfloat deltaTime) {
		if (p->locked) {
			return; // locked particles do not move
		}

		glm::vec3 tempPos = p->pos;

		p->pos = p->pos + (c1 * p->v * deltaTime); // x1

		for (int j = 0; j < 3; j++) {

			glm::vec3 F = glm::vec3(0.0f, 0.0f, 0.0f); // force acting upon the particle

			F += (p->m * g); // Add gravity
			F += wind; // Add wind
			//F += airDir * (-air * (p->v * p->v)); // Add air resistance

			// Add spring forces

			for (int i = 0; i < p->cons.size(); i++) {
				Constraint c = p->cons[i];

				glm::vec3 dir = p->pos - c.b->pos;				
				
				if (dir == glm::vec3(0.0f, 0.0f, 0.0f)) {
					dir = p->pos - c.a->pos;
				}
				

				dir = glm::normalize(dir);

				F -= dir * (c.k * (c.length() - c.restLength));
			}

			/*
			p->pos = p->pos + deltaTime * p->v;
			p->v = p->v + deltaTime * F / p->m;
			p->a = F / p->m;
			*/

			glm::vec3 airDir = F;
			airDir = glm::normalize(airDir);

			F += airDir * (-air * (p->v * p->v));

			p->a = F / p->m;

			switch (j)
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

		//MAXIMUM STRETCH DETECTION
		//go through all constraints, adjust if necessary
		if (enableMinMaxStretch) {
			for (int i = 0; i < p->cons.size(); i++) {
				Constraint c = p->cons[i];

				glm::vec3 dir = p->pos - c.b->pos;

				Particle* part = c.b; //other end of contraint

				if (dir == glm::vec3(0.0f, 0.0f, 0.0f)) {
					dir = p->pos - c.a->pos;
					part = c.a;
				}

				dir = glm::normalize(dir);

				if ((c.length() / c.restLength) > 1.1f) {
					p->pos = part->pos + (dir * (c.restLength * 1.1f));
					continue;
				}
				else if ((c.length() / c.restLength) < 0.9f) {
					p->pos = part->pos + (dir * (c.restLength * 0.9f));
					continue;
				}
			}
		}

		// COLLISION DETECTION
		for (int i = 0; i < spheres.size(); i++) {
			Sphere s = spheres.at(i);
			if (s.collidesWith(p)) {
				if (friction == 1) {
					// sticky collisions - return point to where it was
					p->pos = tempPos;
					p->v = glm::vec3(0.f, 0.f, 0.f);
				}
				else {
					// slippery collisions - push point away from sphere
					glm::vec3 normal = glm::normalize(p->pos - s.pos); // normal at point of collision
					p->pos = s.pos + (normal * s.r);
				}
				continue;
			}
		}
		

		newP->pos = p->pos;
		newP->v = p->v;
		p->pos = tempPos;

		return;
	}

	int fixSprings(Particle* p) {
		if (p->locked) {
			return 0; // locked particles do not move
		}

		int ret = 0;

		for (int i = 0; i < p->cons.size(); i++) {
			Constraint c = p->cons[i];

			glm::vec3 dir = p->pos - c.b->pos;

			Particle* part = c.b; //other end of contraint

			if (dir == glm::vec3(0.0f, 0.0f, 0.0f)) {
				dir = p->pos - c.a->pos;
				part = c.a;
			}

			dir = glm::normalize(dir);

			if ((c.length() / c.restLength) > 1.1f) {
				p->pos = part->pos + (dir * (c.restLength * 1.1f));
				ret = 1;
			}
			else if ((c.length() / c.restLength) < 0.9f) {
				p->pos = part->pos + (dir * (c.restLength * 0.9f));
				ret = 1;
			}

		}

		return ret;
	}
}

#endif
#include "physics/particle_system.h"

ParticleSystem2D::ParticleSystem2D(int numParticles, float radius) :
	_numParticles(numParticles),
	_particleRadius(radius) {

	_particles = new Particle2D[MAX_PARTICLES];
	arrangeParticles();
}

ParticleSystem2D::~ParticleSystem2D() {
	delete _particles;
}

void ParticleSystem2D::arrangeParticles() {
	// TODO: Add these to an ImGui window later to play with on the fly
	float spacing = _particleRadius;
	glm::vec2 offset = glm::vec2(0.0f);

	// Calculate the size of the grid based on how many particles we have
	int gridSize = static_cast<int>(glm::ceil(glm::sqrt(_numParticles)));
	offset = glm::vec2((gridSize - 1.0f) / -2.0f); // Center the grid around the origin

	for (int i = 0; i < _numParticles; i++) {
		// Arrange the positions of the particles into grids
		_particles[i].position.x = static_cast<float>((i) % gridSize) * spacing + offset.x;
		_particles[i].position.y = static_cast<float>((i) / gridSize) * spacing + offset.y;
	}
}

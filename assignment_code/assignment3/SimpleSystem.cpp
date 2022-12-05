#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include "SimpleSystem.hpp"



namespace GLOO {

ParticleState SimpleSystem::ComputeTimeDerivative(const ParticleState& state, float time) const {
    glm::vec3 dx(-state.positions[0][1], state.positions[0][0], 0.f);
    glm::vec3 dv(-state.velocities[0][1], state.velocities[0][0], 0.f);
    ParticleState delta = ParticleState();
    delta.positions.push_back(dx);
    delta.velocities.push_back(dv);
    return delta;
}
}
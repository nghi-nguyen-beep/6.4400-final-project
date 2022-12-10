#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include "SakuraSystem.hpp"

namespace GLOO {

ParticleState SakuraSystem::ComputeTimeDerivative(const ParticleState& state, float time) const {
    ParticleState delta;
    glm::vec3 wind_force(0.3f, .05f, 0.f);
    for (int i = 0; i < state.positions.size(); i++) {
        glm::vec3 gravity(0.f, -masses[i] * kGravity, 0.f);
        float x_rand = -0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.1f - (-0.1f))));
        float y_rand = -0.4f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.8f - (-0.3f))));
        float z_rand = -0.05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.05f - (-0.05f))));

        glm::vec3 dx(((state.velocities[i] * wind_force) + glm::vec3(0.f, y_rand, 0.f)));
        glm::vec3 dv((gravity + wind_force + glm::vec3(x_rand, y_rand, z_rand)) / masses[i]);
        delta.positions.push_back(dx);
        delta.velocities.push_back(dv);
        delta.lifetimes.push_back(state.lifetimes[i]);
    }
    return delta;
}

void SakuraSystem::AddMass(float m) {
    masses.push_back(m);
}

}
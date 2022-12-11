#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include "SakuraSystem.hpp"

namespace GLOO {

ParticleState SakuraSystem::ComputeTimeDerivative(const ParticleState& state, float time) const {
    ParticleState delta;
    glm::vec3 wind_force(0.3f, .05f, 0.f);
    for (int i = 0; i < state.positions.size(); i++) {
        glm::vec3 gravity(0.f, -masses[i] * kGravity, 0.f);
        if (state.positions[i][1] <= -1. ) { // y at bottom
            delta.positions.push_back(glm::vec3(0.f));
            delta.velocities.push_back(glm::vec3(0.f));
        } else {
            float x_rand = -0.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.1f - (-0.1f))));
            float y_rand = -0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.f - (-0.5f))));
            float z_rand = -0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.5f - (-0.5f))));

            glm::vec3 dx(((state.velocities[i] * wind_force) + glm::vec3(0.f, y_rand, 0.f)));
            glm::vec3 dv((gravity + wind_force + glm::vec3(x_rand, y_rand, z_rand)) / masses[i]);
            delta.positions.push_back(dx);
            delta.velocities.push_back(dv);
        }
        delta.lifetimes.push_back(state.lifetimes[i]);
    }
    return delta;
}

void SakuraSystem::AddMass(float m) {
    masses.push_back(m);
}

}
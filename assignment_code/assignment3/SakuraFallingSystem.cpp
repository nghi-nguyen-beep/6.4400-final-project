#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include "SakuraFallingSystem.hpp"

namespace GLOO {

ParticleState SakuraFallingSystem::ComputeTimeDerivative(const ParticleState& state, float time) const {
    ParticleState delta;
    glm::vec3 wind_force;

    for (int i = 0; i < state.positions.size(); i++) {
        if (wind.size() > 0 && wind[0]) {
            wind_force = glm::vec3(0.05f, .005f, 0.f);
        } else {
            wind_force = glm::vec3 (0.f, 0.f, 0.f);
        }
        if (state.positions[i][1] <= -1. ) { // y at bottom
            delta.positions.push_back(glm::vec3(0.f));
            delta.velocities.push_back(glm::vec3(0.f));
        } else {
            glm::vec3 drag_force(state.velocities[i] * -k);
            glm::vec3 gravity(0.f, -masses[i] * kGravity, 0.f);
            float x_rand = -0.7f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.7f - (-0.7f))));
            float y_rand = -0.4f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.8f - (-0.3f))));
            float z_rand = -0.05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.05f - (-0.05f))));

            glm::vec3 dx((state.velocities[i] + glm::vec3(x_rand, 0.f, z_rand)));
            glm::vec3 dv((gravity + drag_force + wind_force) / masses[i]);
            delta.positions.push_back(dx);
            delta.velocities.push_back(dv);
        }
        delta.lifetimes.push_back(state.lifetimes[i]);
    }
    return delta;
}

void SakuraFallingSystem::AddMass(float m) {
    masses.push_back(m);
}

void SakuraFallingSystem::AddWind(bool b) {
    if (wind.size() == 0) {
        wind.push_back(b);
    } else {
        wind[0] = b;
    }
}

}
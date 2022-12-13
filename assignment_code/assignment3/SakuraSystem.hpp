#ifndef SAKURA_SYSTEM_H_
#define SAKURA_SYSTEM_H_

#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"

namespace GLOO {
class SakuraSystem : public ParticleSystemBase {
 public:
   ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const override;
   void AddMass(float m);
 private:
    float k = 0.03f;
    std::vector<float> masses;
};
}  // namespace GLOO

#endif

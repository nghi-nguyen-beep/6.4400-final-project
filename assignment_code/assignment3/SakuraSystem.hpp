#ifndef SAKURA_SYSTEM_H_
#define SAKURA_SYSTEM_H_

#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"

namespace GLOO {
class SakuraSystem : public ParticleSystemBase {
 public:
   ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const override;
 private:
};
}  // namespace GLOO

#endif

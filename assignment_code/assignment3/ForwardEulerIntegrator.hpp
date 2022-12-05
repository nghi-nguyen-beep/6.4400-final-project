#ifndef FORWARD_EULER_INTEGRATOR_H_
#define FORWARD_EULER_INTEGRATOR_H_

#include "IntegratorBase.hpp"
#include "ParticleSystemBase.hpp"
#include "IntegratorFactory.hpp"
#include <iostream>

namespace GLOO {
template <class TSystem, class TState>
class ForwardEulerIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    TState dx = system.ComputeTimeDerivative(state, start_time); // f(x, t)
    dx *= dt;
    TState new_state = state + dx;

    return new_state;
  }
};
}  // namespace GLOO

#endif

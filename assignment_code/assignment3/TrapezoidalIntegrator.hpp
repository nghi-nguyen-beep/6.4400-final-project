#ifndef TRAPEZOIDAL_INTEGRATOR_H_
#define TRAPEZOIDAL_INTEGRATOR_H_

#include "IntegratorBase.hpp"
#include "ParticleSystemBase.hpp"
#include "IntegratorFactory.hpp"

namespace GLOO {
template <class TSystem, class TState>
class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {

    TState f0 = system.ComputeTimeDerivative(state, start_time); // f(x, t)
    TState f0h = f0 * dt;
    TState f1 = system.ComputeTimeDerivative(state + f0h, start_time + dt); // f(x + h * f0, t + h)

    TState new_state = state + dt/2 * (f0 + f1);
    return new_state;
  }
};
}  // namespace GLOO

#endif

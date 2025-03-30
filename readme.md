# Overview
Source code for simulation projects developed while reading Computer Simulation Using Particles by RW Hockney and JW Eastwood.

# Models
## Basic Particle-Particle Model
`basic_pp_model.c` is the simplest particle to particle simulation of charged particles. Coulomb's Law is used as the force law.

**Simulation pseudo code**
```
state:
    N <- particle count
    f <- force accumulators
    x <- positions
    v <- velocities
    q <- electric charges
    t <- simulation time

1. Compute forces.
    Clear force accumulators
    for each i in { 0..N }
        for each j in { i..N }
            f_ij <- Force between particles i and j
            state.f[i] += f_ij
            state.f[i] -= f_ij

2. Integrate equations of motion
    dt <- time between simulation steps
    mass <- particle mass
    for each i in { 0..N }
        state.x[i] += state.v[i] * dt
        state.v[i] += state.f[i] / mass * dt

3. Update time counter
    t += dt
```

![basic_pp_model](attachments/basic_pp_model.gif)


# Double Pendulum — Numerical Simulation

A C++ visualization of a planar double pendulum. The equations of motion are written as a four-variable first-order system, integrated with the GSL ODE driver using its RK4 stepper, and rendered with SFML 3.

**Project status:** learning / exploratory nonlinear-dynamics project. It visualizes the trajectory but does not yet perform a systematic chaos or energy-conservation analysis.

## Project timeline

- **Documented by:** June 2025

## Physical model

Two point masses $`m_1`$ and $`m_2`$ are connected by massless rigid rods of lengths $`L_1`$ and $`L_2`$, with the first rod attached to a fixed pivot. The angles $`\theta_1`$ and $`\theta_2`$ are measured from the downward vertical. The state vector used by the code is

```math
\mathbf{y} =
\begin{pmatrix}
\theta_1\\
\theta_2\\
\omega_1\\
\omega_2
\end{pmatrix},
\qquad
\omega_i = \dot{\theta}_i.
```

The first two equations are kinematic:

```math
\dot{\theta}_1 = \omega_1,
\qquad
\dot{\theta}_2 = \omega_2.
```

The remaining two components calculate the coupled angular accelerations. With

```math
D = 2m_1 + m_2 - m_2\cos\left(2(\theta_1-\theta_2)\right),
```

the implementation evaluates

```math
\begin{aligned}
\dot{\omega}_1 ={}&
\frac{
-g(2m_1+m_2)\sin\theta_1
-m_2g\sin(\theta_1-2\theta_2)
}{L_1D}\\
&-
\frac{
2m_2\sin(\theta_1-\theta_2)
\left[
\omega_2^2L_2+
\omega_1^2L_1\cos(\theta_1-\theta_2)
\right]
}{L_1D}.
\end{aligned}
```

```math
\dot{\omega}_2 =
\frac{2\sin(\theta_1-\theta_2)}{L_2D}
\left[
\omega_1^2L_1(m_1+m_2)
+g(m_1+m_2)\cos\theta_1
+\omega_2^2L_2m_2\cos(\theta_1-\theta_2)
\right].
```

These are the standard coupled equations for the ideal planar double pendulum under the stated angle convention.

## Numerical method

The state is advanced by GSL's `gsl_odeiv2_driver` with the RK4 stepper. The initial step size is `dt = 0.01`; the absolute and relative tolerances supplied to the GSL driver are both `1e-6`. Each rendering-loop iteration requests an advance from the current time $`t`$ to $`t+\Delta t`$.

Current initial conditions and parameters:

| Quantity | Value |
|---|---:|
| Masses | m<sub>1</sub> = 1, m<sub>2</sub> = 1 |
| Rod lengths | L<sub>1</sub> = 1, L<sub>2</sub> = 1 |
| Gravity | g = 9.81 |
| Initial angle | &theta;<sub>1</sub>(0) = &pi;/4 |
| Initial angle | &theta;<sub>2</sub>(0) = &pi; |
| Initial angular velocities | &omega;<sub>1</sub>(0) = 0, &omega;<sub>2</sub>(0) = 0 |
| Requested time increment | 0.01 |

The screen coordinates are computed from the angles using

```math
\begin{aligned}
x_1 &= x_p + L_1\sin\theta_1,
&\qquad y_1 &= y_p + L_1\cos\theta_1,\\
x_2 &= x_1 + L_2\sin\theta_2,
&\qquad y_2 &= y_1 + L_2\cos\theta_2.
\end{aligned}
```

with a display scale factor. The window draws the pivot, two bobs, and the connecting rods. The title is updated with simulation time and both angles.

## Build and run (Ubuntu)

Dependencies: C++20 compiler, CMake, SFML 3, and GNU GSL.

```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev libgsl-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/double_pendulum
```

The SFML package must provide **SFML 3**. The display is limited to approximately 100 frames/s to match the requested 0.01 simulation-time increment per frame.

## Limitations and next steps

- The simulation does not currently calculate total mechanical energy. Add kinetic and potential energy diagnostics to monitor numerical drift.
- Compare RK4 step sizes and tolerances, and check convergence of trajectories over a fixed time interval.
- Nearby initial conditions can separate rapidly in chaotic regimes. To quantify this, evolve two trajectories with a small initial angular separation and measure their separation over time before saturation.
- The current initial condition is a single demonstration, not a parameter survey.
- The render loop draws the state before advancing it, so the visual update follows the most recently completed integration step.

## Software references

- GNU Scientific Library (GSL): https://www.gnu.org/software/gsl/
- SFML: https://www.sfml-dev.org/

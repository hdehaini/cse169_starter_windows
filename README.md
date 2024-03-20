# Cloth Simulation Project

## Overview
This project presents a realistic cloth simulation using a particle system framework. The cloth is modeled using interconnected particles, spring-dampers, and triangular surfaces to simulate the fabric's physics accurately. It includes the effects of gravity, spring elasticity, damping, aerodynamic drag, and collisions with a simple ground plane.

## Demo

[![Cloth Simulation](demo\cloth.jpg)](https://youtu.be/_hYTnRiXC-Q)

## Features
- **Realistic Cloth Dynamics**: Simulates cloth behavior using particles interconnected by spring-dampers, responding realistically to physical forces.
- **Environmental Interactions**: Includes uniform gravity, adjustable wind speed for aerodynamic effects, and ground plane collisions.
- **User Interaction**: Allows manipulation of the cloth by moving fixed particles, enabling dynamic control of the cloth's position and shape in the simulation environment.
- **Configurable Cloth Properties**: Provides controls to adjust physical properties like spring elasticity, damping, and wind speed to observe different behaviors of the cloth simulation.

## Technical Details
- Separate classes for `Particle`, `SpringDamper`, and `Triangle` implement the specific physics for each component, while a `Cloth` class manages these elements to form the complete simulation.
- The simulation includes mechanisms for calculating normal vectors for realistic rendering and aerodynamic calculations, with support for dynamic smooth shading.
- User controls are implemented for interactive manipulation of the cloth, particularly for moving fixed points and adjusting ambient wind speed.

## Installation
Instructions on how to set up and run the project, including required software or libraries.

```bash
git clone https://github.com/hdehaini/cse169_starter_windows/tree/cloth-sim
cd ./cse169_starter_windows/build/Debug
./menv.exe

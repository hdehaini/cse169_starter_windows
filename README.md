# Particle System Project

## Overview
This project is a general-purpose particle system simulator designed to demonstrate various physics principles, including gravity, aerodynamic drag, and collision dynamics. It features a graphical user interface (GUI) that allows for real-time interaction and adjustment of numerous particle attributes.

## Demo
[![Particle System demo](demo\thumbnail.png)](demo\video.mp4)

## Features
- **Dynamic Particle Simulation**: Continuously creates and destroys particles with a customizable rate, simulating real-world particle behavior.
- **Physics-Based Interactions**: Incorporates gravity, aerodynamic drag, and ground collision effects to create a realistic particle movement experience.
- **Adjustable Particle Properties**: Users can interactively modify various particle parameters, such as creation rate, initial position, velocity, lifespan, and physical properties like gravity, air density, and drag coefficient.
- **Graphical User Interface**: A user-friendly interface that displays the simulation in real-time and allows for immediate adjustments to the particle system parameters.

## Adjustable Properties
- Particle creation rate (particles per second)
- Initial position & variance (X, Y, Z)
- Initial velocity & variance (X, Y, Z)
- Initial life span & variance
- Gravity
- Air density
- Drag coefficient
- Particle radius (for both rendering & aerodynamic drag)
- Collision elasticity
- Collision friction

## Getting Started
To run this particle system simulation, clone the repository to your local machine and follow the setup instructions provided in the installation section.

```bash
git clone https://github.com/hdehaini/cse169_starter_windows/tree/particle-system
cd C:\Users\hsdeh\OneDrive\Documents\coding_projects_laptop\CSE-169\cse169_starter_windows\build\Debug
.\menv.exe

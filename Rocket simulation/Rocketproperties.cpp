#include "Rocket.h"

Rocket::Rocket()
    :position(0.0f, 0.0f, 0.0f),  // Start at origin (x=0, y=0, z=0)
    velocity(0.0f, 0.0f, 0.0f),  // No initial movement
    fuel(100.0f),                // Start with full fuel
    gravity(-9.81f),             // Gravity pulling downward along y-axis
    engineThrust{ 0.0f, 0.0f, 0.0f, 0.0f } // Initialize engine thrusts to 0
{}

void Rocket::applyThrust(std::array<float, 4> thrustValues, float deltaTime) {
    if (fuel > 0.0f) {
        // Calculate total thrust from all engines
        float totalThrust = 0.0f;
        for (int i = 0; i < 4; i++) {
            engineThrust[i] = thrustValues[i];
            totalThrust += engineThrust[i];
        }

        // Apply the total thrust to increase the rocket's upward velocity (along y-axis)
        velocity.y += totalThrust * deltaTime;

        // Decrease fuel based on total thrust
        fuel -= totalThrust * 0.1f * deltaTime;
        if (fuel < 0.0f) {
            fuel = 0.0f;  // Ensure fuel doesn't drop below 0
        }
    }
}

void Rocket::update(float deltaTime) {
    // Apply gravity to the rocket's velocity
    velocity.y += gravity * deltaTime;

    // Update the rocket's position based on its velocity
    position += velocity * deltaTime;

    // Prevent the rocket from falling below the ground (y = 0)
    if (position.y < 0) {
        position.y = 0;
        velocity.y = 0;
    }
}

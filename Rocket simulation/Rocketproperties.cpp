#include "Rocket.h"

Rocket::Rocket()
    :position(0.0f, 0.0f, 0.0f),  // Start at origin (x=0, y=0, z=0)
    velocity(0.0f, 0.0f, 0.0f),  // No initial movement
    fuel(100.0f),                // Start with full fuel
    gravity(-9.81f)              // Gravity pulling downward along y-axis
{}

void Rocket::applyThrust(float thrustValue, float deltaTime) {
    if (fuel > 0.0f) {
        // Apply thrust to increase the rocket's upward velocity (along y-axis)
        velocity.y += thrustValue * deltaTime;
        // Decrease fuel based on the thrust value
        fuel -= thrustValue * 0.1f * deltaTime;
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

    // Prevent the rocket from falling below the ground (y = 
}
#include "Rocket.h"

Rocket::Rocket()
    :position(0.0f, 0.0f, 0.0f),  // Start at origin (x=0, y=0, z=0)
     velocity(0.0f, 0.0f, 0.0f),  // No initial movement
     fuel(100.0f),                // Start with full fuel
     thrust(0.0f),                // No thrust initially
     gravity(-9.81f)              // Gravity pulling downward along y-axis
{}

void Rocket::applyThrust(float thrustValue) {
    if (fuel > 0.0f) {
        thrust = thrustValue;
        velocity.y += thrustValue;
        fuel -= thrustValue * 0.1f;

    }
}

void Rocket::update(float deltaTime) {
    velocity.y += gravity * deltaTime;
    position += velocity * deltaTime;


    if (position.y < 0) {
        position.y = 0;
        velocity.y = 0;

    }
}
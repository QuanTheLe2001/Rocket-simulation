#ifndef ROCKET_H
#define ROCKET_H

#include <glm.hpp>

class Rocket {
public:
    glm::vec3 position;  // Rocket's position in 3D space (x, y, z)
    glm::vec3 velocity;  // Rocket's velocity (movement per unit time)
    float fuel;          // Rocket's fuel level
    float gravity;       // Gravity constant

    // Constructor
    Rocket();

    // Apply thrust to the rocket
    void applyThrust(float thrustValue, float deltaTime);

    // Update the rocket's physics (called every frame)
    void update(float deltaTime);
};

#endif

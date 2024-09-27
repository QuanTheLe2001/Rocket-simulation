#ifndef ROCKET_H
#define ROCKET_H

#include <glm.hpp>
#include <array>

class Rocket {
public:
    glm::vec3 position;  // Rocket's position in 3D space (x, y, z)
    glm::vec3 velocity;  // Rocket's velocity (movement per unit time)
    float fuel;          // Rocket's fuel level
    float gravity;       // Gravity constant

    std::array<float, 4> engineThrust; // Thrust for each engine (4 engines)

    // Constructor
    Rocket();

    // Apply thrust to the rocket
    void applyThrust(std::array<float, 4> thrustValues, float deltaTime);

    // Update the rocket's physics (called every frame)
    void update(float deltaTime);
};

#endif

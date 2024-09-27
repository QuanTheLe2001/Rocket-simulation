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

    // Thrust levels for 5 engines
    std::array<float, 5> engineThrust; // Use std::array for thrust values

    // Constructor
    Rocket();

    // Apply thrust to the rocket, accepting std::array<float, 5>
    void applyThrust(const std::array<float, 5>& thrustValues, float deltaTime);

    // Update the rocket's physics (called every frame)
    void update(float deltaTime);
};

#endif

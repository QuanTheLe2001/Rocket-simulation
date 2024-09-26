#include <GLFW/glfw3.h>
#include <iostream>
#include "Rocket.h"

const GLint WIDTH = 1280, HEIGHT = 720;

int main() {
	//Ininialize GLFW, if GLFW initialize fail, exit the program
	if (!glfwInit()) {
		std::cerr << "Fail to Initialize " << std::endl;
		return -1;

	}
	
	// create a GLFW window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rocket Simulation", nullptr, nullptr);
	if (!window) {
		std::cerr << "GLFW error" << std::endl;
		glfwTerminate();
		return -1;
	}

    glfwMakeContextCurrent(window);

    // Create an instance of the Rocket class
    Rocket rocket;

    // Time handling variables for deltaTime calculation
    float previousTime = glfwGetTime();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Handle input events
        glfwPollEvents();

        // Calculate deltaTime for smooth physics updates
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        // Apply thrust when the spacebar is pressed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            rocket.applyThrust(1.0f);  // Apply upward thrust
        }
        else {
            rocket.applyThrust(0.0f);  // No thrust when the spacebar is not pressed
        }

        // Update the rocket's physics
        rocket.update(deltaTime);

        // Render code would go here (we will add 3D rendering later)

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap buffers to display the current frame
        glfwSwapBuffers(window);
    }

    // Clean up and terminate
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include "Rocket.h"

const GLint WIDTH = 1280, HEIGHT = 720;

float fuelLevel = 100.0f;  // Full fuel (100%)
float altitude = 0.0f;     // Starting altitude
float speed = 0.0f;        // Rocket speed (m/s)
float thrustLevel = 50.0f; // Thrust level (0 to 100%)

bool isLiftoffInitiated = false;
bool isLiftoffComplete = false;
float liftoffStartTime = 0.0f;

Rocket rocket;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rocket Simulation with Countdown and Dynamic UI", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    float previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Calculate deltaTime for smooth physics updates
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        // Liftoff countdown logic
        if (isLiftoffInitiated && !isLiftoffComplete) {
            if (currentTime - liftoffStartTime >= 10.0f) {
                isLiftoffComplete = true;
                std::cout << "Liftoff complete!" << std::endl;
            }
        }

        // Apply thrust and update rocket physics if liftoff is complete
        if (isLiftoffComplete && fuelLevel > 0.0f && rocket.position.y < 10000.0f) {
            rocket.applyThrust(thrustLevel, deltaTime);
            rocket.update(deltaTime);

            // Update fuel level based on thrust
            fuelLevel -= thrustLevel * 0.05f * deltaTime;
            if (fuelLevel < 0.0f) fuelLevel = 0.0f;

            // Update altitude and speed
            altitude = rocket.position.y;
            speed = rocket.velocity.y;
        }

        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main UI Window
        ImGui::Begin("Rocket Simulation Control Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(1280, 720));
        ImGui::SetWindowPos(ImVec2(0, 0));

        // 1. Top Panel for Mission Controls
        if (ImGui::CollapsingHeader("Mission Controls")) {
            ImGui::Text("Mission: CRS-17");
            if (ImGui::Button("Liftoff")) {
                if (!isLiftoffInitiated) {
                    isLiftoffInitiated = true;
                    isLiftoffComplete = false;
                    liftoffStartTime = currentTime;
                    rocket = Rocket(); // Reset rocket state
                    fuelLevel = 100.0f; // Reset fuel level
                    altitude = 0.0f; // Reset altitude
                    speed = 0.0f; // Reset speed
                    std::cout << "Liftoff initiated, countdown started!" << std::endl;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Abort")) {
                isLiftoffInitiated = false; // Cancel liftoff
                fuelLevel = 0.0f; // Abort stops fuel consumption
            }
        }

        // 2. Left Column for Engine Status and Fuel Tanks
        ImGui::Columns(3, "columns", false);

        // Engine Stats (Column 1)
        ImGui::BeginChild("Engines", ImVec2(0, 400), true);
        ImGui::Text("Engine #1");
        ImGui::ProgressBar(thrustLevel / 100.0f, ImVec2(0.0f, 0.0f), "Throttle: 72%");
        ImGui::ProgressBar(thrustLevel / 100.0f, ImVec2(0.0f, 0.0f), "Thrust: 663.3N");
        ImGui::Separator();

        ImGui::Text("Engine #2");
        ImGui::ProgressBar(thrustLevel / 100.0f, ImVec2(0.0f, 0.0f), "Throttle: 72%");
        ImGui::ProgressBar(thrustLevel / 100.0f, ImVec2(0.0f, 0.0f), "Thrust: 663.3N");
        ImGui::EndChild();

        ImGui::NextColumn();

        // Fuel Tanks (Column 2)
        ImGui::BeginChild("FuelTanks", ImVec2(0, 400), true);
        ImGui::Text("Fuel Tanks");

        // Use dynamic labels for fuel level
        std::string loxLabel = "LOX: " + std::to_string(fuelLevel) + "%";
        std::string rp1Label = "RP-1: " + std::to_string(fuelLevel) + "%";

        // Progress bars for fuel tanks (continuously rendered)
        ImGui::ProgressBar(fuelLevel / 100.0f, ImVec2(0.0f, 0.0f), loxLabel.c_str());
        ImGui::ProgressBar(fuelLevel / 100.0f, ImVec2(0.0f, 0.0f), rp1Label.c_str());

        ImGui::EndChild();

        ImGui::NextColumn();

        // Progress and Flight Data (Column 3)
        ImGui::BeginChild("FlightData", ImVec2(0, 400), true);
        ImGui::Text("Flight Data");
        ImGui::Text("Flight Time: %.2f seconds", currentTime);
        ImGui::Text("Speed: %.2f m/s", speed);
        ImGui::Text("Altitude: %.2f m", altitude);
        ImGui::Text("Thrust Level: %.1f%%", thrustLevel);

        // Liftoff countdown display
        if (isLiftoffInitiated && !isLiftoffComplete) {
            ImGui::Text("Liftoff in %.1f seconds...", 10.0f - (currentTime - liftoffStartTime));
        }
        else if (isLiftoffComplete) {
            ImGui::Text("Liftoff!");
        }

        ImGui::EndChild();

        ImGui::Columns(1);

        ImGui::End();  // End the main window

        // Render the UI
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers to display the rendered frame
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

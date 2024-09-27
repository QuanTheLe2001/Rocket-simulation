#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <array>
#include "Rocket.h"

const GLint WIDTH = 1280, HEIGHT = 720;

float fuelLevel = 100.0f;  // Full fuel (100%)
float altitude = 0.0f;     // Starting altitude
float speed = 0.0f;        // Rocket speed (m/s)
float acceleration = 0.0f; // Rocket acceleration (m/s^2)
float currentMass = 500.0f; // Initial mass of the rocket (kg)
const float DRY_MASS = 100.0f; // Mass of the rocket when empty (no fuel)
const float MAX_ALTITUDE = 10000.0f; // Maximum altitude for simulation
const float MAX_ACCELERATION = 20.0f; // Maximum acceleration value for display
std::array<float, 5> thrustLevels = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f }; // Initial thrust for 5 engines

bool isLiftoffInitiated = false;
bool isLiftoffComplete = false;
float liftoffStartTime = 0.0f;

Rocket rocket;

// Progress state variables
enum ProgressState { LOAD_FUEL, COUNTDOWN, START_ENGINES, LIFTOFF };
ProgressState currentProgress = LOAD_FUEL;

// Function to draw vertical bar
void DrawVerticalBar(float level, ImVec2 pos, ImVec2 size, ImU32 color) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 topLeft = pos;
    ImVec2 bottomRight = ImVec2(pos.x + size.x, pos.y + size.y);

    // Draw the outer bar border
    drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 255, 255));

    // Calculate the height of the filled portion
    float filledHeight = size.y * (level / 100.0f);

    // Draw the filled portion (top to bottom)
    ImVec2 filledTopLeft = ImVec2(pos.x, pos.y + size.y - filledHeight);
    ImVec2 filledBottomRight = ImVec2(pos.x + size.x, pos.y + size.y);
    drawList->AddRectFilled(filledTopLeft, filledBottomRight, color);
}

// Helper function to display flight data with colors
void DisplayFlightDataWithColor(const char* label, float value, const char* unit, ImU32 color) {
    ImGui::Text(label);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%.2f %s", value, unit);
    ImGui::PopStyleColor();
}

// Function to render the Flight Progress Panel
void RenderFlightProgressPanel() {
    ImGui::BeginChild("FlightProgressPanel", ImVec2(0, 500), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::Text("Flight Progress");

    // Display flight data with different colors
    DisplayFlightDataWithColor("Acceleration:", acceleration, "m/s^2", IM_COL32(255, 215, 0, 255));  // Yellow for Acceleration
    DisplayFlightDataWithColor("Speed:", speed, "m/s", IM_COL32(255, 69, 0, 255));  // Red for Speed
    DisplayFlightDataWithColor("Altitude:", altitude / 1000.0f, "km", IM_COL32(50, 205, 50, 255));  // Green for Altitude
    ImGui::Separator();

    // Additional Data
    ImGui::Text("Downrange: 0.0 km");
    ImGui::Text("Traveled Distance: %.3f km", altitude / 1000.0f);
    ImGui::Text("Mach: %.3f", speed / 343.0f);
    ImGui::Text("Angular Accel.: 0.0 mrad/s^2");

    ImGui::EndChild();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rocket Simulation", nullptr, nullptr);
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
            currentProgress = COUNTDOWN;
            if (currentTime - liftoffStartTime >= 10.0f) {
                isLiftoffComplete = true;
                currentProgress = LIFTOFF;
                std::cout << "Liftoff complete!" << std::endl;
            }
        }

        // Apply thrust if liftoff is complete and fuel is available
        if (isLiftoffComplete && fuelLevel > 0.0f) {
            rocket.applyThrust(thrustLevels, deltaTime); // Apply thrust to all engines
            rocket.update(deltaTime);
            currentProgress = START_ENGINES;

            // Update fuel level based on thrust and decrease mass accordingly
            float totalThrust = thrustLevels[0] + thrustLevels[1] + thrustLevels[2] + thrustLevels[3] + thrustLevels[4];
            float fuelConsumption = totalThrust * 0.05f * deltaTime;
            fuelLevel -= fuelConsumption;
            if (fuelLevel < 0.0f) fuelLevel = 0.0f;

            // Update the mass of the rocket as fuel burns
            currentMass = DRY_MASS + (fuelLevel / 100.0f) * (500.0f - DRY_MASS); // Dry mass + remaining fuel mass

            // Calculate acceleration based on thrust and current mass
            acceleration = totalThrust / currentMass;

            // Clamp acceleration to maximum value for visual representation
            if (acceleration > MAX_ACCELERATION) acceleration = MAX_ACCELERATION;

            // Update altitude and speed
            altitude = rocket.position.y;
            speed = rocket.velocity.y;

            // Ensure the altitude bar fills up to the maximum defined value
            if (altitude > MAX_ALTITUDE) altitude = MAX_ALTITUDE;

            // Dynamically adjust thrust levels based on altitude
            for (int i = 0; i < thrustLevels.size(); i++) {
                thrustLevels[i] = 80.0f + 20.0f * (rocket.position.y / MAX_ALTITUDE);
            }
        }

        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main UI Window
        ImGui::Begin("Rocket Simulation Control Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(1280, 720));
        ImGui::SetWindowPos(ImVec2(0, 0));

        // Launch and Abort Buttons
        ImGui::SetCursorPos(ImVec2(10, 20));
        if (ImGui::Button("Launch", ImVec2(100, 40))) {
            if (!isLiftoffInitiated) {
                isLiftoffInitiated = true;
                isLiftoffComplete = false;
                liftoffStartTime = currentTime;
                rocket = Rocket(); // Reset rocket state
                fuelLevel = 100.0f; // Reset fuel level
                currentMass = 500.0f; // Reset mass
                altitude = 0.0f; // Reset altitude
                speed = 0.0f; // Reset speed
                currentProgress = LOAD_FUEL;
                std::cout << "Liftoff initiated, countdown started!" << std::endl;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Abort", ImVec2(100, 40))) {
            isLiftoffInitiated = false;
            fuelLevel = 0.0f;
            currentProgress = LOAD_FUEL;
        }

        ImGui::Columns(4, "columns", false);

        // Combined Child Window for Engines and Fuel Tanks
        ImGui::BeginChild("EngineSection", ImVec2(0, 500), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::Text("Engine Status and Fuel Tanks");
        for (int i = 0; i < 5; i++) {
            std::string engineLabel = "Engine #" + std::to_string(i + 1);
            std::string thrustLabel = "Throttle: " + std::to_string(static_cast<int>(thrustLevels[i])) + "%";
            ImGui::Text("%s", engineLabel.c_str());
            ImGui::ProgressBar(thrustLevels[i] / 100.0f, ImVec2(0.0f, 0.0f), thrustLabel.c_str());
            ImGui::Separator();
        }

        ImVec2 barSize = ImVec2(30, 150);
        ImGui::Text("Fuel Tanks:");
        ImGui::SetCursorPos(ImVec2(30, 300));
        DrawVerticalBar(fuelLevel, ImGui::GetCursorScreenPos(), barSize, IM_COL32(0, 255, 0, 255));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + barSize.y + 5);
        ImGui::Text("LOX");

        ImGui::SetCursorPos(ImVec2(100, 300));
        DrawVerticalBar(fuelLevel, ImGui::GetCursorScreenPos(), barSize, IM_COL32(0, 255, 0, 255));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + barSize.y + 5);
        ImGui::Text("RP-1");

        ImGui::SetCursorPos(ImVec2(170, 300));
        DrawVerticalBar((altitude / MAX_ALTITUDE) * 100.0f, ImGui::GetCursorScreenPos(), barSize, IM_COL32(255, 165, 0, 255));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + barSize.y + 5);
        ImGui::Text("Altitude");

        ImGui::SetCursorPos(ImVec2(240, 300));
        DrawVerticalBar((acceleration / MAX_ACCELERATION) * 100.0f, ImGui::GetCursorScreenPos(), barSize, IM_COL32(255, 69, 0, 255));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + barSize.y + 5);
        ImGui::Text("Acceleration");

        ImGui::EndChild();

        ImGui::NextColumn();

        // Flight Data and Status
        ImGui::BeginChild("FlightData", ImVec2(0, 500), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::Text("Flight Data");
        ImGui::Text("Flight Time: %.2f seconds", currentTime);
        ImGui::Text("Speed: %.2f m/s", speed);
        ImGui::Text("Altitude: %.2f m", altitude);
        ImGui::Text("Acceleration: %.2f m/s^2", acceleration);

        float totalThrust = thrustLevels[0] + thrustLevels[1] + thrustLevels[2] + thrustLevels[3] + thrustLevels[4];
        ImGui::Text("Total Thrust Level: %.1f%%", totalThrust);

        if (isLiftoffInitiated && !isLiftoffComplete) {
            ImGui::Text("Liftoff in %.1f seconds...", 10.0f - (currentTime - liftoffStartTime));
        }
        else if (isLiftoffComplete) {
            ImGui::Text("Liftoff!");
        }

        ImGui::EndChild();

        ImGui::NextColumn();

        // New Flight Progress Panel
        RenderFlightProgressPanel();

        ImGui::NextColumn();

        // Progress Panel
        ImGui::BeginChild("ProgressPanel", ImVec2(0, 500), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::Text("Progress");

        // Dynamically highlight based on the state
        ImGui::PushStyleColor(ImGuiCol_Button, currentProgress >= LOAD_FUEL ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 100, 100, 255));
        ImGui::Button("Load Fuel", ImVec2(-1, 0));
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Button, currentProgress >= COUNTDOWN ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 100, 100, 255));
        ImGui::Button("Countdown", ImVec2(-1, 0));
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Button, currentProgress >= START_ENGINES ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 100, 100, 255));
        ImGui::Button("Start Engines", ImVec2(-1, 0));
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Button, currentProgress >= LIFTOFF ? IM_COL32(30, 144, 255, 255) : IM_COL32(255, 100, 100, 255));  // Blue for Liftoff
        ImGui::Button("Liftoff", ImVec2(-1, 0));
        ImGui::PopStyleColor();

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

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

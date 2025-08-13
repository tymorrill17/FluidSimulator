#include <iostream>
#include <sstream>
#include "utility/window.h"
#include "utility/camera.h"
#include "utility/timer.h"
#include "utility/gui.h"
#include "utility/input_manager.h"
#include "renderer/renderer.h"
#include "renderer/descriptor.h"
#include "renderer/buffer.h"
#include "physics/particle_system.h"
#include "physics/hand.h"
#include "render_systems/particle_render_system.h"
#include "render_systems/render_system.h"
#include "render_systems/gui_render_system.h"
#include "application.h"
#include <thread>
#include <chrono>

static const uint32_t APPLICATION_WIDTH = 1920;
static const uint32_t APPLICATION_HEIGHT = 1080;

static const float coordinateScale = 4.5;

struct GlobalUBO {
	glm::mat4 projection;
	glm::mat4 view;
	float aspectRatio;
};

int main(int argc, char* argv[]) {
	// Initialize the renderer, window and input manager
	//Application* app = new Application();
	Application* app = new Application(APPLICATION_WIDTH, APPLICATION_HEIGHT, "2D Fluid Simulator");

	try {

		static Timer& timer = Timer::getTimer();
		static Gui& gui = Gui::getGui();

		// Initialize the rendering descriptor pool
		std::vector<PoolSizeRatio> renderDescriptorSetSizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
			//{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
			//{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10},
			//{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
		};
		DescriptorPool globalDescriptorPool(app->renderer().device(), 10, renderDescriptorSetSizes);

		float particleColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// The particle info struct contains the Particle struct (pos and vel), as well as color and radius of each particle
		GlobalParticleInfo particleInfo{
			.defaultColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			.radius = 0.03f,
			.spacing = 0.025f,
			.numParticles = 1600
		};

		GlobalPhysicsInfo physicsInfo{
			.gravity = 0.f,
			.boundaryDampingFactor = 0.9f,
			.collisionDampingFactor = 0.9f,
			.densitySmoothingRadius = 0.3f,
			.pressureConstant = 20.f,
			.restDensity = 5.f,
			.nSubsteps = 1,
		};

		BoundingBox box{};

		float handRadius = 1.f;
		float interactionStrength = 50.f;
		Hand mouseInteraction(handRadius, interactionStrength, coordinateScale);

		// The constructor of the particle system initializes the positions of the particles to a grid
		ParticleSystem2D fluidParticles(particleInfo, physicsInfo, box, app->inputManager(), &mouseInteraction);

		// We will use a uniform buffer for the global particle info
		Buffer globalParticleBuffer(app->renderer().device(), app->renderer().allocator(), sizeof(GlobalParticleInfo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, app->renderer().device().physicalDeviceProperies().limits.minUniformBufferOffsetAlignment);
		globalParticleBuffer.map();

		// For the actual particle info, we want to use a storage buffer
		Buffer particleBuffer(app->renderer().device(), app->renderer().allocator(), sizeof(Particle2D) * MAX_PARTICLES, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, app->renderer().device().physicalDeviceProperies().limits.minStorageBufferOffsetAlignment);
		particleBuffer.map();

		Buffer globalBuffer(app->renderer().device(), app->renderer().allocator(), sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, app->renderer().device().physicalDeviceProperies().limits.minUniformBufferOffsetAlignment);
		globalBuffer.map();

		VkDescriptorSetLayout particleLayouts = app->renderer().descriptorLayoutBuilder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();
		VkDescriptorSet particleDescriptor = globalDescriptorPool.allocateDescriptorSet(particleLayouts);
		app->renderer().descriptorWriter().addBufferWrite(0, globalParticleBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER).addBufferWrite(1, particleBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER).updateDescriptorSet(particleDescriptor).clear();
		app->renderer().descriptorLayoutBuilder().clear();

		VkDescriptorSetLayout globalLayout = app->renderer().descriptorLayoutBuilder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();
		VkDescriptorSet globalDescriptor = globalDescriptorPool.allocateDescriptorSet(globalLayout);
		app->renderer().descriptorWriter().addBufferWrite(0, globalBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER).updateDescriptorSet(globalDescriptor).clear();

		// Create the render systems and add them to the renderer
		ParticleRenderSystem particleRenderSystem(app->renderer(), std::vector<VkDescriptorSetLayout>{particleLayouts, globalLayout}, std::vector<VkDescriptorSet>{particleDescriptor, globalDescriptor}, fluidParticles);
		app->renderer().addRenderSystem(&particleRenderSystem);

		// Set up the camera
		Camera camera{};

		GlobalUBO globalBufferObject{};

		GuiRenderSystem guiRenderSystem(app->renderer(), app->window());
		app->renderer().addRenderSystem(&guiRenderSystem);

        std::cout << "Starting the main loop!" << std::endl;

		// Start physics when this becomes true;
		bool letThereBeLight = false;
		glm::vec2 mousePosition;

		// Main application loop
		while (!app->window().shouldClose()) {
			app->inputManager().processInputs(); // Poll the user inputs
			if (app->window().pauseRendering()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				timer.update(); // Updating the timer here too so there are no large jumps in timer updates
				continue;
			}

			timer.update();
			guiRenderSystem.getNewFrame();

			// Timer Info Display
			gui.addWidget("Info", [&]() {
				ImGui::Text("FrameTime: %.8f ms", timer.frameTime());
				ImGui::Text("FPS: %.2f", timer.framesPerSecond());
				ImGui::Text("Mouse Position: (%.2f, %.2f)", mousePosition.x, mousePosition.y);
				});

			gui.addWidget("Controls", [&]() {
				if (ImGui::Button("Start")) {
					letThereBeLight = true;
				}
				if (ImGui::Button("Reset")) {
					letThereBeLight = false;
				}
				});

			// Particle Info Display
			gui.addWidget("Particle Info", [&]() {
				ImGui::DragFloat("Radius", &particleInfo.radius, 0.001, 0.0f, 1000000.0f);
				ImGui::DragFloat("Spacing", &particleInfo.spacing, 0.001, 0.0f, 1000000.0f);
				ImGui::DragInt("# Particles", &particleInfo.numParticles, 1, 0, MAX_PARTICLES);
				ImGui::ColorEdit4("Default Color", particleInfo.defaultColor);
				});

			// Physics Info Display
			gui.addWidget("Physics Info", [&]() {
				ImGui::DragFloat("Gravity", &physicsInfo.gravity, 0.01, 0.0f, 1000000.0f);
				ImGui::DragFloat("Boundary Damping", &physicsInfo.boundaryDampingFactor, 0.001, 0.0f, 1.0f);
				ImGui::DragFloat("Collision Damping", &physicsInfo.collisionDampingFactor, 0.001, 0.0f, 1.0f);
				ImGui::DragFloat("Density Smoothing", &physicsInfo.densitySmoothingRadius, 0.001, 0.01f, 10.f);
				ImGui::DragFloat("Pressure Constant", &physicsInfo.pressureConstant, 0.01, 0.01f, 1000.f);
				ImGui::DragFloat("Rest Density", &physicsInfo.restDensity, 0.01, 0.01f, 10000.f);
				ImGui::DragInt("# Substeps", &physicsInfo.nSubsteps, 1, 1, 100);
				});

			gui.addWidget("Interaction", [&]() {
				ImGui::DragFloat("Radius", &handRadius, 0.001f, 0.001f, 1000000.0f);
				ImGui::DragFloat("Strength", &interactionStrength, 0.001f, 0.001f, 1000000.0f);
				ImGui::Text("Position: (%.2f, %.2f)", mouseInteraction.position().x, mouseInteraction.position().y);
				});

			// Set the camera projection with the current aspect ratio
			float aspect = app->window().aspectRatio();
			box.left = -aspect * coordinateScale; box.right = aspect * coordinateScale;
			box.bottom = -1.0f * coordinateScale; box.top = 1.0f * coordinateScale;
			//camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, 0.1f, 10.0f);
			camera.setOrthographicProjection(box.left, box.right, box.bottom, box.top, 0.1f, 10.0f);
			camera.setViewDirection(glm::vec3{ 0.0f, 0.0f, -2.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f });

			// Update camera info in the global buffer
			globalBufferObject.aspectRatio = aspect;
			globalBufferObject.projection = camera.projectionMatrix();
			globalBufferObject.view = camera.viewMatrix();

			mousePosition = app->inputManager().mousePosition();
			mouseInteraction.setPosition(mousePosition);
			mouseInteraction.radius = handRadius;
			mouseInteraction.strengthFactor = interactionStrength;

			if (letThereBeLight) {
				fluidParticles.update(); // Update the particle systems
			}
			else {
				fluidParticles.arrangeParticles();
			}

			// HERE is where I would redo the DescriptorWriter calls to updateDescriptors with the updated buffer/offset size?
			// Update/fill buffers
			globalBuffer.writeData(&globalBufferObject);
			globalParticleBuffer.writeData(&particleInfo);
			particleBuffer.writeData(fluidParticles.particles());

			app->renderer().renderAllSystems();

			app->renderer().resizeCallback(); // Check for window resize and call the window resize callback function

			guiRenderSystem.endFrame();
		}

		app->renderer().waitForIdle();

        std::cout << "Shutting Down... Bye Bye!" << std::endl;
	}
	catch (const std::exception& e) {
		std::stringstream line;
		line << "Caught exception: " << e.what();
		std::cout << line.str() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

#pragma once

#include "vulkan/vulkan.h"
#include "render_systems/render_system.h"
#include "renderer/renderer.h"
#include "renderer/device.h"
#include "renderer/pipeline_builder.h"
#include "renderer/command.h"
#include "physics/particle_system.h"

#include <vector>

class ParticleRenderSystem : public RenderSystem {
public:
	ParticleRenderSystem(Renderer& renderer, std::vector<VkDescriptorSetLayout> particleDescriptorLayout, std::vector<VkDescriptorSet> particleDescriptorSets, ParticleSystem2D& particleSystem);

	void render(Command& cmd);

	void bindDescriptor(VkDescriptorSet set);

private:
	ParticleSystem2D& _particleSystem;

	std::vector<Pipeline> _pipelines;
	std::vector<VkDescriptorSetLayout> _particleDescriptors;
	std::vector<VkDescriptorSet> _particleSet;

	void buildPipeline();
};

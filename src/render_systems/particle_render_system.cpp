#include "render_systems/particle_render_system.h"

void ParticleRenderSystem::buildPipeline() {

	_renderer.pipelineBuilder().clear();

	std::string baseDir = static_cast<std::string>(BASE_DIR);
	std::string folderDir = baseDir + "\\shaders\\";

	Shader defaultVertShader(_renderer.device(), folderDir + "circle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	Shader defaultFragShader(_renderer.device(), folderDir + "circle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	Pipeline pipeline = _renderer.pipelineBuilder().setVertexInputState(PipelineBuilder::vertexInputStateCreateInfo())
		.setShader(defaultVertShader)
		.setShader(defaultFragShader)
		.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.setPolygonMode(VK_POLYGON_MODE_FILL)
		.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
		.setMultisampling(VK_SAMPLE_COUNT_1_BIT)
		.setBlending(false)
		.setDepthTest()
		.setColorAttachmentFormat(_renderer.swapchain().imageFormat())
		.addDescriptors(_particleDescriptors)
		.buildPipeline();

	_pipelines.push_back(std::move(pipeline));
}

ParticleRenderSystem::ParticleRenderSystem(Renderer& renderer, std::vector<VkDescriptorSetLayout> particleDescriptorLayout, std::vector<VkDescriptorSet> particleDescriptorSets, ParticleSystem2D& particleSystem) :
	RenderSystem(renderer),
	_particleDescriptors(particleDescriptorLayout),
	_particleSet(particleDescriptorSets),
	_particleSystem(particleSystem) {

	buildPipeline();
}

void ParticleRenderSystem::render(Command& cmd) {

	// Bind pipelines and draw here
	for (auto& pipeline : _pipelines) {
		vkCmdBindPipeline(cmd.buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline());
		vkCmdBindDescriptorSets(cmd.buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout(), 0, static_cast<uint32_t>(_particleSet.size()), _particleSet.data(), 0, nullptr);
	}
	vkCmdDraw(cmd.buffer(), 6*_particleSystem.particleInfo().numParticles, 1, 0, 0);
}

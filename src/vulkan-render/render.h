#ifndef RENDER_H
#define RENDER_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <atomic>

#include <glmhelper.h>

#include "vkinit.h"
#include "vkhelper.h"
#include "render_structs.h"
#include "pipeline.h"
#include "descriptor_sets.h"
#include "texture_loader.h"
#include "texfont.h"
#include "model_loader.h"

const size_t MAX_3D_INSTANCE = 100;
const size_t MAX_2D_INSTANCE = 1500; 

class Render
{
public:
	Render(GLFWwindow* window);
	Render(GLFWwindow* window, glm::vec2 target);
	void setViewMatrixAndFov(glm::mat4 view, float fov);
	~Render();
	void restartResourceLoad();
	Resource::Texture LoadTexture(std::string filepath);
	Resource::Font* LoadFont(std::string filepath);
	Resource::Model LoadModel(std::string filepath);
	void endResourceLoad();

	void begin3DDraw();
	void begin2DDraw();
	void DrawModel(Resource::Model model, glm::mat4 modelMatrix, glm::mat4 normalMatrix);
	void DrawQuad(const Resource::Texture& texture, glm::mat4 modelMatrix, glm::vec4 colour, glm::vec4 texOffset);
	void DrawQuad(const Resource::Texture& texture, glm::mat4 modelMatrix, glm::vec4 colour);
	void DrawQuad(const Resource::Texture& texture, glm::mat4 modelMatrix);
	void DrawString(Resource::Font* font, std::string text, glm::vec2 position, float size, float rotate, glm::vec4 colour);
	void DrawString(Resource::Font* font, std::string text, glm::vec2 position, float size, float rotate, glm::vec4 colour, float depth);
	void endDraw(std::atomic<bool>& submit);
	void set2DViewMatrix(glm::mat4 view);
	void framebufferResize();

private:
	bool mFramebufferResized = false;

	GLFWwindow* mWindow;
	glm::vec2 targetResolution;
	VkInstance mInstance;
	VkSurfaceKHR mSurface;
	Base mBase;
	FrameData mFrame;
	SwapChain mSwapchain;
	VkRenderPass mRenderPass;
	VkRenderPass mFinalRenderPass;

	VkCommandPool mGeneralCommandPool;
	VkCommandBuffer mTransferCommandBuffer;

	Pipeline mPipeline3D;
	Pipeline mPipeline2D;
	Pipeline mPipelineFinal;

	//descriptor set members
	VkDeviceMemory mShaderMemory;
	VkBuffer mShaderBuffer;

	DS::DescriptorSet mVP3Dds;
	DS::DescriptorSet mVP2Dds;
	DS::DescriptorSet mPerInstance3Dds;
	DS::DescriptorSet mPer2DVertds;
	DS::DescriptorSet mLightingds;
	DS::DescriptorSet mTexturesds;
	DS::DescriptorSet mPer2Dfragds;
	DS::DescriptorSet mOffscreends;

	DS::BindingAndData<DS::ShaderStructs::viewProjection> mVP3D;
	DS::BindingAndData<DS::ShaderStructs::viewProjection> mVP2D;
	DS::BindingAndData<DS::ShaderStructs::Per3DInstance> mPerInstance;
	DS::BindingAndData<DS::ShaderStructs::Per2DVert> mPer2Dvert;
	DS::BindingAndData<bool> mTextureViews;
	DS::BindingAndData<bool> mTextureSampler;
	DS::BindingAndData<DS::ShaderStructs::Per2DFrag> mPer2Dfrag;
	DS::BindingAndData<DS::ShaderStructs::lighting> mLighting;
	DS::BindingAndData<bool> mOffscreenSampler;
	DS::BindingAndData<bool> mOffscreenView;
	VkSampler offscreenSampler;

	Resource::TextureLoader mTextureLoader;
	Resource::ModelLoader mModelLoader;

	bool mBegunDraw = false;
	bool mFinishedLoadingResources = false;
	bool m3DRender = true;
	uint32_t mImg;
	VkSemaphore mImgAquireSem;
	float mProjectionFov = 45.0f;

	unsigned int modelRuns = 0;
	unsigned int current3DInstanceIndex = 0;
	Resource::Model currentModel;
	Resource::Texture currentTexture;
	glm::vec4 currentTexOffset = glm::vec4(0, 0, 1, 1);
	glm::vec4 currentColour = glm::vec4(1, 1, 1, 1);

	unsigned int instance2Druns = 0;
	unsigned int current2DInstanceIndex = 0;

	void _initRender(GLFWwindow* window);
	void _initFrameResources();
	void _destroyFrameResources();
	void _startDraw();
	void _resize();
	void _updateViewProjectionMatrix();
	void _drawBatch();

#ifndef NDEBUG
	VkDebugUtilsMessengerEXT mDebugMessenger;
#endif

};





#endif

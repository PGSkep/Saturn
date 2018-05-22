#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../../Common/Utility/Console.h"
#include "Input2.h"
#include "Clock.h"

#include "Renderer.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1);

	Con::SetPosition(-965, 0);
	Con::SetSize(965, 1045);

	Input2::Init("InputName");
	Input2::Reset();
	Input2::Update();

	Clock2::Init();

	Renderer renderer;
	renderer.InitInstance(
		VulkanCommon::GetVkApplicationInfo("Application Name", 0, "Engine Name", 0, VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION)),
		{ "VK_LAYER_LUNARG_standard_validation" },
		{ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME },
		VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,
		VulkanCommon::DebugReportCallback
	);
	renderer.AddSurface("Window", "Title", 1800, 900, VulkanCommon::MainWndProc);
	renderer.InitPhysicalDevice();

	renderer.InitDevice(
		0,
		{ "VK_KHR_swapchain" },
		VulkanCommon::PhysicalDevivceGetFeaturesFromStrArr({ "fillModeNonSolid", "multiViewport", "tessellationShader", "geometryShader", "samplerAnisotropy" }),
		{ Renderer::QueueFamilyRequirements::Get(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, true,{ 1.0f }), }
	);

	renderer.InitPresentation();
	renderer.InitCompute();
	
	renderer.InitMemory();

	renderer.InitFramebuffers();

	renderer.InitSamplers();

	renderer.InitDescriptos();

	renderer.InitShaders();
	renderer.InitPipeline();

	renderer.WriteComputeCommandBuffers();

	Camera camera;
	camera.position = { 0.0f, -120.0f, 60.0f };
	camera.rotation = { 0.0f, 0.0f, 0.0f };
	camera.fov = 45.0f;
	camera.aspect = 1800.0f / 900.0f;
	camera.nearV = 1.0f;
	camera.farV = 10000.0f;
	camera.speed = 10.0f;

	Math3D::Mat4 viewProjection[2];

	float timeMultiplier = 4.0f;
	float timeMin = 0.03125f;
	float timeMax = 1024.00f;

	Clock2::Update();
	while (!Input2::IsDown(Input2::ESC))
	{
		Clock2::Update();
		Input2::Update();

		// DeltaTime
		if (Input2::IsDown(Input2::ARROW_RIGHT))
		{
			timeMultiplier *= 1.05f;

			if (timeMultiplier > timeMax)
				timeMultiplier = timeMax;
		}
		if (Input2::IsDown(Input2::ARROW_LEFT))
		{
			timeMultiplier *= 0.95f;

			if (timeMultiplier < timeMin)
				timeMultiplier = timeMin;
		}

		// ViewProjection
		float camSpeedMult = 50.0f;
		if (Input2::IsDown(Input2::Q))
			camera.position.z -= (float)Clock2::GetDeltaTime() * camSpeedMult;
		if (Input2::IsDown(Input2::E))
			camera.position.z += (float)Clock2::GetDeltaTime() * camSpeedMult;
		if (Input2::IsDown(Input2::A))
			camera.position.x -= (float)Clock2::GetDeltaTime() * camSpeedMult;
		if (Input2::IsDown(Input2::D))
			camera.position.x += (float)Clock2::GetDeltaTime() * camSpeedMult;
		if (Input2::IsDown(Input2::W))
			camera.position.y += (float)Clock2::GetDeltaTime() * camSpeedMult;
		if (Input2::IsDown(Input2::S))
			camera.position.y -= (float)Clock2::GetDeltaTime() * camSpeedMult;

		viewProjection[0] = camera.GetLookAt();
		viewProjection[1] = camera.GetProjectionMatrix();

		renderer.UpdateBuffers((float)Clock2::GetDeltaTime(), timeMultiplier, viewProjection);
		renderer.WriteGraphicsCommandBuffers();
		renderer.Present();
	}

	renderer.ShutDown();

	return 0;
}

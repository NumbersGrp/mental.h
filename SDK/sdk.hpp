/**
 * @file sdk.hpp
 * @brief Main header file for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 * 
 * This file provides the main entry point for the Mental SDK, including
 * all necessary components for window management and rendering.
 */

#pragma once

#include "Renderer/CMentalRenderer.hpp"
#include "Renderer/RenderPipeline/CMentalRenderPipeline.hpp"
#include "Renderer/Object/CMentalTriangle.hpp"
#include "Renderer/Object/CMentalFBXModel.hpp"
#include "Renderer/Object/CMentalOBJModel.hpp"
#include "Renderer/Material/CMentalShader.hpp"
#include "Camera/CMentalCamera.hpp"
#include "Input/CMentalInput.hpp"
#include "UI/CMentalImGui.hpp"
#include "Scripting/CMentalScriptEngine.hpp"
#include "Scripting/CMentalCameraController.hpp"
#include "WindowManager/CMentalWindowManager.hpp"

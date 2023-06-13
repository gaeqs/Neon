/*
 *  Copyright (c) 2015-2023 VG-Lab/URJC.
 *
 *  Authors: Gael Rial Costas  <gael.rial.costas@urjc.es>
 *
 *  This file is part of ViSimpl <https://github.com/vg-lab/visimpl>
 *
 *  This library is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License version 3.0 as published
 *  by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//
// Created by grial on 7/06/23.
//

#include "QTApplication.h"

#include <iostream>
#include <utility>
#include <engine/structure/Room.h>

namespace {
    bool debugFilter(VkDebugReportFlagsEXT flags,
                     VkDebugReportObjectTypeEXT objectType, uint64_t object,
                     size_t location, int32_t messageCode,
                     const char* pLayerPrefix, const char* pMessage) {
        return false;
    }
}

neon::vulkan::QTApplication::QTApplication() :
        _application(nullptr),
        _onInit(),
        _currentFrameInformation(0, 0.016f, 0.0f),
        _lastFrameTime(std::chrono::high_resolution_clock::now()),
        _lastFrameProcessTime(0.0f),
        _currentCommandBuffer(),
        _swapChainCount(0),
        _recording(false) {
}

void neon::vulkan::QTApplication::init(neon::Application* application) {
    _application = application;
}

glm::ivec2 neon::vulkan::QTApplication::getWindowSize() const {
    QSize s = size();
    return {s.width(), s.height()};
}

neon::FrameInformation
neon::vulkan::QTApplication::getCurrentFrameInformation() const {
    return _currentFrameInformation;
}

neon::CommandBuffer*
neon::vulkan::QTApplication::getCurrentCommandBuffer() const {
    if (!isRecordingCommandBuffer()) {
      return nullptr;
    }
    if (_currentCommandBuffer == nullptr) {
        _currentCommandBuffer = std::make_unique<CommandBuffer>(
                _application,
                currentCommandBuffer()
        );
    }
  return _currentCommandBuffer.get();
}

void neon::vulkan::QTApplication::lockMouse(bool lock) {
    // TODO
}

neon::Result<uint32_t, std::string>
neon::vulkan::QTApplication::startGameLoop() {
    return {"QTApplicaiton doesn't contain a GameLoop, as Qt already has one."};
}

void neon::vulkan::QTApplication::renderFrame(neon::Room* room) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - _lastFrameTime;
    _lastFrameTime = now;

    float seconds = static_cast<float>(duration.count()) * 1e-9f;


    _currentFrameInformation = {
            _currentFrameInformation.currentFrame + 1,
            seconds,
            _lastFrameProcessTime
    };

    if (room != nullptr) {
        room->update(_currentFrameInformation.currentDeltaTime);
        room->preDraw();
        room->draw();
        frameReady();
        requestUpdate();
    }

    now = std::chrono::high_resolution_clock::now();
    auto processTime = now - _lastFrameTime;
    _lastFrameProcessTime = static_cast<float>(processTime.count()) * 1e-9f;
    _currentCommandBuffer = nullptr;
}

VkInstance neon::vulkan::QTApplication::getInstance() const {
    return vulkanInstance()->vkInstance();
}

VkDevice neon::vulkan::QTApplication::getDevice() const {
    return device();
}

VkPhysicalDevice neon::vulkan::QTApplication::getPhysicalDevice() const {
    return physicalDevice();
}

VkQueue neon::vulkan::QTApplication::getGraphicsQueue() const {
    return graphicsQueue();
}

VkFormat neon::vulkan::QTApplication::getSwapChainImageFormat() const {
    return colorFormat();
}

VkFormat neon::vulkan::QTApplication::getDepthImageFormat() const {
    return depthStencilFormat();
}

VkCommandPool neon::vulkan::QTApplication::getCommandPool() const {
    return graphicsCommandPool();
}

VkSwapchainKHR neon::vulkan::QTApplication::getSwapChain() const {
    std::cerr << "QTApplication doesn't expose a swap chain!" << std::endl;
    return nullptr;
}

uint32_t neon::vulkan::QTApplication::getMaxFramesInFlight() const {
    return MAX_CONCURRENT_FRAME_COUNT;
}

uint32_t neon::vulkan::QTApplication::getCurrentFrame() const {
    return concurrentFrameCount();
}

uint32_t neon::vulkan::QTApplication::getCurrentSwapChainImage() const {
    return swapChainImageCount();
}

uint32_t neon::vulkan::QTApplication::getSwapChainCount() const {
    return _swapChainCount;
}

VkExtent2D neon::vulkan::QTApplication::getSwapChainExtent() const {
    QSize s = swapChainImageSize();
    return {static_cast<uint32_t>(s.width()),
            static_cast<uint32_t>(s.height())};
}

VkDescriptorPool neon::vulkan::QTApplication::getImGuiPool() const {
    std::cerr << "QTApplication doesn't support ImGUI" << std::endl;
    return nullptr;
}

bool neon::vulkan::QTApplication::isRecordingCommandBuffer() const {
    return _recording;
}

void neon::vulkan::QTApplication::preInitResources() {
    vulkanInstance()->installDebugOutputFilter(debugFilter);
}

void neon::vulkan::QTApplication::initResources() {
    if (_onInit) {
        _onInit(this);
    }
}

void neon::vulkan::QTApplication::initSwapChainResources() {
    ++_swapChainCount;
}

void neon::vulkan::QTApplication::releaseSwapChainResources() {
}

void neon::vulkan::QTApplication::releaseResources() {
}

void neon::vulkan::QTApplication::startNextFrame() {
    _recording = true;
    if (_application != nullptr) {
        renderFrame(_application->getRoom().get());
    }
    _recording = false;
}

void neon::vulkan::QTApplication::physicalDeviceLost() {
}

void neon::vulkan::QTApplication::logicalDeviceLost() {
}

QVulkanWindowRenderer* neon::vulkan::QTApplication::createRenderer() {
    return this;
}

void neon::vulkan::QTApplication::exposeEvent(QExposeEvent* event) {
    QVulkanWindow::exposeEvent(event);
}

void neon::vulkan::QTApplication::resizeEvent(QResizeEvent* event) {
    QVulkanWindow::resizeEvent(event);
}

bool neon::vulkan::QTApplication::event(QEvent* event) {
    return QVulkanWindow::event(event);
}

void neon::vulkan::QTApplication::setInitializationFunction(
        std::function<void(QTApplication * )> func) {
    _onInit = std::move(func);
}

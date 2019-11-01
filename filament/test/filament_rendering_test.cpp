/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <backend/PixelBufferDescriptor.h>

using namespace filament;
using namespace backend;

static void callback(void* buffer, size_t size, void* user) {
    uint8_t const* rgba = (uint8_t const*)buffer;

    EXPECT_EQ(rgba[0], 0xff);
    EXPECT_EQ(rgba[1], 0);
    EXPECT_EQ(rgba[2], 0);
    EXPECT_EQ(rgba[3], 0xff);

    ::free(buffer);
}

TEST(RenderingTest, Clear) {
    Engine* engine = Engine::create();
    SwapChain* surface = engine->createSwapChain(16, 16);
    Renderer* renderer = engine->createRenderer();
    View* view = engine->createView();
    view->setClearColor(LinearColorA{1, 0, 0, 1});

    void* buffer = malloc(16 * 16 * 4);
    PixelBufferDescriptor pd(buffer, 16 * 16 * 4, PixelDataFormat::RGBA, PixelDataType::UBYTE,
            callback, nullptr);

    renderer->beginFrame(surface);
    renderer->render(view);
    renderer->readPixels(0, 0, 16, 16, std::move(pd));
    renderer->endFrame();

    engine->destroy(view);
    engine->destroy(renderer);
    engine->destroy(surface);
    Engine::destroy(&engine);
}

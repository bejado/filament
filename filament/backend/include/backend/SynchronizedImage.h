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

//! \file

#ifndef TNT_FILAMENT_BACKEND_SYNCHRONIZED_IMAGE_H
#define TNT_FILAMENT_BACKEND_SYNCHRONIZED_IMAGE_H

#include <utils/compiler.h>

#include <stddef.h>
#include <stdint.h>

namespace filament {
namespace backend {

class UTILS_PUBLIC SynchronizedImage {
public:
    /**
     * Callback used to release the image.
     * It is guaranteed to be called on the main filament thread.
     */
    using Callback = void(*)(void* image, void* user);

    //! creates an empty descriptor
    SynchronizedImage() noexcept = default;

    //! calls the callback to advertise SynchronizedImage no longer owns the image
    ~SynchronizedImage() noexcept {
        if (callback) {
            callback(image, user);
        }
    }

    SynchronizedImage(const SynchronizedImage& rhs) = delete;
    SynchronizedImage& operator=(const SynchronizedImage& rhs) = delete;

    SynchronizedImage(SynchronizedImage&& rhs) noexcept
        : image(rhs.image), callback(rhs.callback), user(rhs.user) {
            rhs.image = nullptr;
            rhs.callback = nullptr;
    }

    SynchronizedImage& operator=(SynchronizedImage&& rhs) noexcept {
        if (this != &rhs) {
            image = rhs.image;
            callback = rhs.callback;
            user = rhs.user;
            rhs.image = nullptr;
            rhs.callback = nullptr;
        }
        return *this;
    }

    SynchronizedImage(void const* image,
            Callback callback = nullptr, void* user = nullptr) noexcept
                : image(const_cast<void*>(image)), callback(callback), user(user) {
    }

    /**
     * Set or replace the release callback function
     * @param callback  The new callback function
     * @param user      An opaque user pointer passed to the callbeck function when it's called
     */
    void setCallback(Callback callback, void* user = nullptr) noexcept {
        this->callback = callback;
        this->user = user;
    }

    //! Returns whether a release callback is set
    bool hasCallback() const noexcept { return callback != nullptr; }

    //! Returns the currently set release callback function
    Callback getCallback() const noexcept {
        return callback;
    }

    //! Returns the user opaque pointer associated to this BufferDescriptor
    void* getUser() const noexcept {
        return user;
    }

    void* image = nullptr;

private:
    // callback when the image is consumed.
    Callback callback = nullptr;
    void* user = nullptr;
};

} // namespace backend
} // namespace filament

#endif // TNT_FILAMENT_BACKEND_SYNCHRONIZED_IMAGE_H

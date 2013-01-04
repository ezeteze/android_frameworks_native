/*
 * Copyright (C) 2007 The Android Open Source Project
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

#define LOG_TAG "GraphicBufferMapper"
#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#include <stdint.h>
#include <errno.h>

#include <utils/Errors.h>
#include <utils/Log.h>
#include <utils/Trace.h>

#include <ui/GraphicBufferMapper.h>
#include <ui/Rect.h>

#include <hardware/gralloc.h>


namespace android {
// ---------------------------------------------------------------------------

ANDROID_SINGLETON_STATIC_INSTANCE( GraphicBufferMapper )

GraphicBufferMapper::GraphicBufferMapper()
    : mAllocMod(0)
{
    hw_module_t const* module;
    int err = hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module);
    ALOGE_IF(err, "FATAL: can't find the %s module", GRALLOC_HARDWARE_MODULE_ID);
    if (err == 0) {
        mAllocMod = (gralloc_module_t const *)module;
    }
}

status_t GraphicBufferMapper::registerBuffer(buffer_handle_t handle)
{
    ATRACE_CALL();
    status_t err;

    err = mAllocMod->registerBuffer(mAllocMod, handle);

    ALOGW_IF(err, "registerBuffer(%p) failed %d (%s)",
            handle, err, strerror(-err));
    return err;
}

status_t GraphicBufferMapper::unregisterBuffer(buffer_handle_t handle)
{
    ATRACE_CALL();
    status_t err;

    err = mAllocMod->unregisterBuffer(mAllocMod, handle);

    ALOGW_IF(err, "unregisterBuffer(%p) failed %d (%s)",
            handle, err, strerror(-err));
    return err;
}

status_t GraphicBufferMapper::lock(buffer_handle_t handle, 
        int usage, const Rect& bounds, void** vaddr)
{
    ATRACE_CALL();
    status_t err;
#ifdef NO_HWCOMPOSER
        int tries=5;
#endif

    err = mAllocMod->lock(mAllocMod, handle, usage,
            bounds.left, bounds.top, bounds.width(), bounds.height(),
            vaddr);
#ifdef NO_HWCOMPOSER
    while (err && tries) {
        sleep(1000);
        err = mAllocMod->unlock(mAllocMod, handle);
        err = mAllocMod->lock(mAllocMod, handle, usage,
                                       bounds.left, bounds.top, bounds.width(), bounds.height(),
                                       vaddr);
        tries--;
        }
#endif

    ALOGW_IF(err, "lock(...) failed %d (%s)", err, strerror(-err));
    return err;
}

status_t GraphicBufferMapper::unlock(buffer_handle_t handle)
{
    ATRACE_CALL();
    status_t err;

    err = mAllocMod->unlock(mAllocMod, handle);

    ALOGW_IF(err, "unlock(...) failed %d (%s)", err, strerror(-err));
    return err;
}

#ifdef EXYNOS4210_ENHANCEMENTS
status_t GraphicBufferMapper::getphys(buffer_handle_t handle, void** paddr)
{
    status_t err;

    err = mAllocMod->getphys(mAllocMod, handle, paddr);

    ALOGW_IF(err, "getphys(%p) fail %d(%s)",
            handle, err, strerror(-err));
    return err;
}
#endif

// ---------------------------------------------------------------------------
}; // namespace android

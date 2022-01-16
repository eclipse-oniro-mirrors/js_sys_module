/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/native_api.h"
#include "native_engine.h"
#include "napi/native_node_api.h"
#include "utils/log.h"

namespace OHOS::Js_sys_module::Dfx {
    static napi_value DumpHeapSnapShot(napi_env env, napi_callback_info info)
    {
        size_t argc = 2;
        size_t requireArgc = 2;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        NAPI_ASSERT(env, argc > requireArgc, "Wrong number of arguments");
        napi_value *argv = nullptr;
        argv = new napi_value[argc];
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        char *tempStr = nullptr;
        size_t tempStrsize = 0;
        napi_get_value_string_utf8(env, argv[0], nullptr, 0, &tempStrsize);
        tempStr = new char[tempStrsize + 1];
        napi_get_value_string_utf8(env, argv[0], tempStr, tempStrsize + 1, &tempStrsize);
        std::string pathStr = tempStr;
        bool isVmMode = true;
        napi_get_value_bool(env, argv[1], &isVmMode);
        delete []tempStr;
        delete []argv;
        argv = nullptr;
        tempStr = nullptr;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        engine->DumpHeapSnapShot(pathStr, isVmMode);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value BuildNativeAndJsBackStackTrace(napi_env env, napi_callback_info info)
    {
        napi_value result = nullptr;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        std::string stackTraceResult = engine->BuildNativeAndJsBackStackTrace();
        NAPI_CALL(env, napi_create_string_utf8(env, stackTraceResult.c_str(), stackTraceResult.size(), &result));
        return result;
    }

    static napi_value StartHeapTracking(napi_env env, napi_callback_info info)
    {
        size_t argc = 2;
        size_t requireArgc = 2;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        NAPI_ASSERT(env, argc > requireArgc, "Wrong number of arguments");
        napi_value *argv = nullptr;
        argv = new napi_value[argc];
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        double timeInterval = 0;
        napi_get_value_double(env, argv[0], &timeInterval);
        bool isVmMode = true;
        napi_get_value_bool(env, argv[1], &isVmMode);
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto startResult = engine->StartHeapTracking(timeInterval, isVmMode);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, startResult, &result));
        return result;
    }

    static napi_value StopHeapTracking(napi_env env, napi_callback_info info)
    {
        size_t argc = 1;
        size_t requireArgc = 1;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        NAPI_ASSERT(env, argc > requireArgc, "Wrong number of arguments");
        napi_value *argv = nullptr;
        argv = new napi_value[argc];
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        char *tempStr = nullptr;
        size_t tempStrsize = 0;
        napi_get_value_string_utf8(env, argv[0], nullptr, 0, &tempStrsize);
        tempStr = new char[tempStrsize + 1];
        napi_get_value_string_utf8(env, argv[0], tempStr, tempStrsize + 1, &tempStrsize);
        std::string filePath = tempStr;
        delete []tempStr;
        delete []argv;
        argv = nullptr;
        tempStr = nullptr;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto stopResult = engine->StopHeapTracking(filePath);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, stopResult, &result));
        return result;
    }

    static napi_value DfxInit(napi_env env, napi_value exports)
    {
        static napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("dumpHeapSnapShot", DumpHeapSnapShot),
            DECLARE_NAPI_FUNCTION("buildNativeAndJsBackStackTrace", BuildNativeAndJsBackStackTrace),
            DECLARE_NAPI_FUNCTION("startHeapTracking", StartHeapTracking),
            DECLARE_NAPI_FUNCTION("stopHeapTracking", StopHeapTracking),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }
    // dfx module define
    static napi_module dfxModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = DfxInit,
        .nm_modname = "dfx",
        .nm_priv = ((void*)0),
        .reserved = {0},
    };

    // dfx module register
    extern "C"
    __attribute__((constructor)) void RegisterModule()
    {
        napi_module_register(&dfxModule);
    }
}
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

#ifndef BASE_COMPILERUNTIME_JS_SYS_MODULE_PROCESS_CLASS_H
#define BASE_COMPILERUNTIME_JS_SYS_MODULE_PROCESS_CLASS_H

#include <cstring>
#include <map>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
namespace OHOS::Js_sys_module::Process {
class Process {
public:
    explicit Process(napi_env env);
    virtual~Process() {}
    napi_value GetUid() const;
    napi_value GetGid() const;
    napi_value GetEUid() const;
    napi_value GetEGid() const;
    napi_value GetGroups() const;
    napi_value GetPid() const;
    napi_value GetPpid() const;
    void Chdir(napi_value args) const;
    napi_value Uptime() const;
    napi_value Kill(napi_value proid, napi_value signal);
    void Abort() const;
    void On(napi_value str, napi_value function);
    napi_value Off(napi_value str);
    void Exit(napi_value number) const;
    napi_value Cwd() const;      
private:
    napi_env env;
    std::map<std::string, napi_value> map_event_;
};
}
#endif
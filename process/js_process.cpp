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

#include "js_process.h"

#include <ctime>
#include <vector>

#include <grp.h>
#include <pwd.h>
#include <uv.h>

#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

#include "securec.h"
#include "utils/log.h"
namespace OHOS::Js_sys_module::Process {
    namespace {
        constexpr int NUM_OF_DATA = 4;
        constexpr int MAX_PATH = 260;
    }
Process::Process(napi_env env_) : env(env_) {}
napi_value Process::GetUid() const
{
    napi_value result = nullptr;
    auto processGetuid = static_cast<uint32_t>(getuid());
    NAPI_CALL(env, napi_create_uint32(env, processGetuid, &result));
    return result;
}

napi_value Process::GetGid() const
{
    napi_value result = nullptr;
    auto processGetgid = static_cast<uint32_t>(getgid());
    NAPI_CALL(env, napi_create_uint32(env, processGetgid, &result));
    return result;
}

napi_value Process::GetEUid() const
{
    napi_value result = nullptr;
    auto processGeteuid = static_cast<uint32_t>(geteuid());
    NAPI_CALL(env, napi_create_uint32(env, processGeteuid, &result));
    return result;
}

napi_value Process::GetEGid() const
{
    napi_value result = nullptr;
    auto processGetegid = static_cast<uint32_t>(getegid());
    NAPI_CALL(env, napi_create_uint32(env, processGetegid, &result));
    return result;
}

napi_value Process::GetGroups() const
{
    napi_value result = nullptr;
    int progroups = getgroups(0, nullptr);
    if (progroups == -1) {
        napi_throw_error(env, "-1", "getgroups initialize failed");
    }
    std::vector<gid_t> pgrous(progroups);
    progroups = getgroups(progroups, pgrous.data());
    if (progroups == -1) {
        napi_throw_error(env, "-1", "getgroups");
    }
    pgrous.resize(progroups);
    gid_t proegid = getegid();
    if (std::find(pgrous.begin(), pgrous.end(), proegid) == pgrous.end()) {
        pgrous.push_back(proegid);
    }
    std::vector<uint32_t> arry;
    for (auto iter = pgrous.begin(); iter != pgrous.end(); iter++) {
        auto recive = static_cast<uint32_t>(*iter);
        arry.push_back(recive);
    }
    NAPI_CALL(env, napi_create_array(env, &result));
    size_t len = arry.size();
    for (size_t i = 0; i < len; i++) {
        napi_value numvalue = nullptr;
        NAPI_CALL(env, napi_create_uint32(env, arry[i], &numvalue));
        NAPI_CALL(env, napi_set_element(env, result, i, numvalue));
    }
    return result;
}

napi_value Process::GetPid() const
{
    napi_value result = nullptr;
    auto proPid = static_cast<int32_t>(getpid());
    napi_create_int32(env, proPid, &result);
    return result;
}

napi_value Process::GetPpid() const
{
    napi_value result = nullptr;
    auto proPpid = static_cast<int32_t>(getppid());
    napi_create_int32(env, proPpid, &result);
    return result;
}

void Process::Chdir(napi_value args) const
{
    size_t prolen = 0;
    napi_get_value_string_utf8(env, args, nullptr, 0, &prolen);
    char* path = nullptr;
    if (prolen > 0) {
        path = new char[prolen + 1];
        if (memset_s(path, prolen + 1, '\0', prolen + 1) != 0) {
            napi_throw_error(env, "-1", "chdir path memset_s failed");
        }
    } else {
        napi_throw_error(env, "-2", "prolen is error !");
    }
    napi_get_value_string_utf8(env, args, path, prolen + 1, &prolen);
    int proerr = 0;
    if (path != nullptr) {
        proerr = uv_chdir(path);
        delete []path;
    }

    if (proerr) {
        napi_throw_error(env, "-1", "chdir");
    }
}

napi_value Process::Kill(napi_value proid, napi_value signal)
{
    int32_t pid = 0;
    int32_t sig = 0;
    napi_get_value_int32(env, proid, &pid);
    napi_get_value_int32(env, signal, &sig);
    uv_pid_t ownPid = uv_os_getpid();
    if (sig > 64 &&
        (pid == 0 || pid == -1 || pid == ownPid || pid == -ownPid)) {
        napi_throw_error(env, "0", "process exit");
    }
    bool flag = false;
    int err = uv_kill(pid, sig);
    if (!err) {
        flag = true;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, flag, &result));
    return result;
}

napi_value Process::Uptime() const
{
    napi_value result = nullptr;
    struct sysinfo information;
    time_t systimer = 0;
    double runsystime = 0.0;
    if (sysinfo(&information)) {
        napi_throw_error(env, "-1", "Failed to get sysinfo");
    }
    systimer = information.uptime;
    if (systimer > 0) {
        runsystime = static_cast<double>(systimer);
        NAPI_CALL(env, napi_create_double(env, runsystime, &result));
    } else {
        napi_throw_error(env, "-1", "Failed to get systimer");
    }
    return result;
}

void Process::Exit(napi_value number) const
{
    int32_t result = 0;
    napi_get_value_int32(env, number, &result);
    exit(result);
}

napi_value Process::Cwd() const
{
    napi_value result = nullptr;
    char buf[MAX_PATH * NUM_OF_DATA] = { 0 };
    size_t length = sizeof(buf);
    int err = uv_cwd(buf, &length);
    if (err) {
        napi_throw_error(env, "1", "uv_cwd");
    }
    napi_create_string_utf8(env, buf, length, &result);
    return result;
}

void Process::Abort() const
{
    abort();
}

void Process::On(napi_value str, napi_value function)
{
    char *buffer = nullptr;
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, str, buffer, 0, &bufferSize);
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1];
    }
    napi_get_value_string_utf8(env, str, buffer, bufferSize + 1, &bufferSize);
    if (buffer != nullptr) {
        map_event_[buffer] = function;
        delete []buffer;
    }
}
napi_value Process::Off(napi_value str)
{
    char *buffer = nullptr;
    size_t bufferSize = 0;
    bool flag = true;
    napi_value result = nullptr;
    napi_get_value_string_utf8(env, str, buffer, 0, &bufferSize);
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1];
    }
    napi_get_value_string_utf8(env, str, buffer, bufferSize + 1, &bufferSize);
    std::string temp = buffer;
    if (buffer != nullptr) {
        delete[] buffer;
    }
    for (auto iter = map_event_.cbegin(); iter != map_event_.cend(); ++iter) {
        if (iter->first == temp) {
            map_event_.erase(temp);
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
    }
    flag = false;
    NAPI_CALL(env, napi_get_boolean(env, flag, &result));
    return result;

}
}
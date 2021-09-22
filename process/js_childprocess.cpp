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

#include "js_childprocess.h"

#include <map>
#include <vector>

#include <csignal>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "securec.h"
#include "utils/log.h"

namespace OHOS::Js_sys_module::Process {
    std::map<std::string, int> g_signalsMap = {
        {"SIGHUP", 1},
        {"SIGINT", 2},
        {"SIGQUIT", 3},
        {"SIGILL", 4},
        {"SIGTRAP", 5},
        {"SIGABRT", 6},
        {"SIGBUS", 7},
        {"SIGFPE", 8},
        {"SIGKILL", 9},
        {"SIGUSR1", 10},
        {"SIGSEGV", 11},
        {"SIGUSR2", 12},
        {"SIGPIPE", 13},
        {"SIGALRM", 14},
        {"SIGTERM", 15},
        {"SIGSTKFLT", 16},
        {"SIGCHLD", 17},
        {"SIGCONT", 18},
        {"SIGSTOP", 19},
        {"SIGTSTP", 20},
        {"SIGTTIN", 21},
        {"SIGTTOU", 22},
        {"SIGURG", 23},
        {"SIGXCPU", 24},
        {"SIGXFSZ", 25},
        {"SIGVTALRM", 26},
        {"SIGPROF", 27},
        {"SIGWINCH", 28},
        {"SIGIO", 29},
        {"SIGPWR", 30},
        {"SIGSYS", 31}
    };
    ChildProcess::ChildProcess(napi_env env) : env_(env) {}
    void ChildProcess::Spawn(napi_value command)
    {
        int ret = pipe(stdOutFd_);
        if (ret < 0) {
            HILOG_ERROR("pipe1 faile %{public}d", errno);
            return;
        }
        ret = pipe(stdErrFd_);
        if (ret < 0) {
            HILOG_ERROR("pipe2 faile %{public}d", errno);
            return;
        }
        pid_t pid = fork();
        if (pid == 0) {
            close(stdErrFd_[0]);
            close(stdOutFd_[0]);
            dup2(stdOutFd_[1], 1); // 1:stdout
            dup2(stdErrFd_[1], 2); // 2:stderr
            std::string strCommnd = RequireStrValue(command);
            if (execl("/bin/sh", "sh", "-c", strCommnd.c_str(), NULL) == -1) {
                HILOG_ERROR("execl command failed");
                exit(127); // 127:if exec fail exitcode is 127
            }
        } else if (pid > 0) {
            optionsInfo_->pid = pid;
            ppid_ = getpid();
            CreatePromise();

            napi_value resourceName = nullptr;
            napi_create_string_utf8(env_, "TimeoutListener", strlen("TimeoutListener"), &resourceName);
            napi_create_async_work(
                env_, nullptr, resourceName, TimeoutListener,
                [](napi_env env, napi_status status, void *data) {
                    OptionsInfo *optionsInfo = (OptionsInfo*)data;
                    napi_delete_async_work(env, optionsInfo->worker);
                    delete optionsInfo;
                },
                (void*)optionsInfo_, &optionsInfo_->worker);
            napi_queue_async_work(env_, optionsInfo_->worker);
            close(stdErrFd_[1]);
            close(stdOutFd_[1]);
        } else {
            HILOG_ERROR("child process create failed");
        }
    }

    napi_value ChildProcess::Wait()
    {
        napi_value promise = nullptr;
        auto waitInfo = new WaitInfo;
        napi_create_promise(env_, &(waitInfo->deferred), &promise);

        if (isWait_) {
            int32_t status;
            isWait_ = false;
            waitpid(optionsInfo_->pid, &status, 0);
            exitCode_ = status;
        }
        isNeedRun_ = false;
        napi_value result = nullptr;
        napi_create_int32(env_, exitCode_, &result);
        napi_resolve_deferred(env_, waitInfo->deferred, result);
        delete waitInfo;
        waitInfo = nullptr;

        return promise;
    }

    napi_value ChildProcess::GetOutput() const
    {
        return stdOutInfo_->promise;
    }

    napi_value ChildProcess::GetErrorOutput() const
    {
        return stdErrInfo_->promise;
    }

    napi_value ChildProcess::GetKilled() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_get_boolean(env_, killed_, &result));

        return result;
    }

    napi_value ChildProcess::Getpid() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_int32(env_, optionsInfo_->pid, &result));

        return result;
    }

    napi_value ChildProcess::Getppid() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_int32(env_, ppid_, &result));

        return result;
    }

    napi_value ChildProcess::GetExitCode() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_int32(env_, exitCode_, &result));

        return result;
    }

    void ChildProcess::CreatePromise()
    {
        // getstdout
        napi_value resourceName = nullptr;
        stdOutInfo_ = new StdInfo();
        stdOutInfo_->isNeedRun = &isNeedRun_;
        stdOutInfo_->fd = stdOutFd_[0];
        stdOutInfo_->pid = optionsInfo_->pid;
        stdOutInfo_->maxBuffSize = optionsInfo_->maxBuffer;
        napi_create_promise(env_, &stdOutInfo_->deferred, &stdOutInfo_->promise);
        napi_create_string_utf8(env_, "ReadStdOut", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env_, nullptr, resourceName, ReadStdOut, EndStdOut,
                               (void*)stdOutInfo_, &stdOutInfo_->worker);
        napi_queue_async_work(env_, stdOutInfo_->worker);

        // getstderr
        resourceName = nullptr;
        stdErrInfo_ = new StdInfo();
        stdErrInfo_->isNeedRun = &isNeedRun_;
        stdErrInfo_->fd = stdErrFd_[0];
        stdErrInfo_->pid = optionsInfo_->pid;
        stdErrInfo_->maxBuffSize = optionsInfo_->maxBuffer;
        napi_create_promise(env_, &stdErrInfo_->deferred, &stdErrInfo_->promise);
        napi_create_string_utf8(env_, "ReadStdErr", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env_, nullptr, resourceName, ReadStdErr, EndStdErr,
                               (void*)stdErrInfo_, &stdErrInfo_->worker);
        napi_queue_async_work(env_, stdErrInfo_->worker);
    }

    void ChildProcess::ReadStdOut(napi_env env, void *data)
    {
        auto stdOutInfo = (StdInfo*)data;
        char childStdout[MAXSIZE] = {0};
        while (*(stdOutInfo->isNeedRun)) {
            read(stdOutInfo->fd, childStdout, sizeof(childStdout) - 1);
            if (strlen(childStdout) > 0) {
                stdOutInfo->stdData += childStdout;
            }
            if (stdOutInfo->stdData.size() > stdOutInfo->maxBuffSize && *(stdOutInfo->isNeedRun)) {
                if (!kill(stdOutInfo->pid, SIGKILL)) {
                    *(stdOutInfo->isNeedRun) = false;
                    stdOutInfo->stdData = stdOutInfo->stdData.substr(0, stdOutInfo->maxBuffSize);
                } else {
                    HILOG_ERROR("stdOut maxBuff kill signal failed");
                }
            }
            if (memset_s(childStdout, MAXSIZE, '\0', MAXSIZE - 1) != 0) {
                HILOG_ERROR("getOutput memset_s failed");
                return;
            }
        }
    }

    void ChildProcess::EndStdOut(napi_env env, napi_status status, void *buffer)
    {
        auto stdOutInfo = (StdInfo*)buffer;
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = stdOutInfo->stdData.size() + 1;
        napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
        if (memcpy_s(data, bufferSize, (const void*)stdOutInfo->stdData.c_str(),
            stdOutInfo->stdData.size()) != 0) {
            HILOG_ERROR("getOutput memcpy_s failed");
            napi_delete_async_work(env, stdOutInfo->worker);
            return;
        }

        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
        napi_resolve_deferred(env, stdOutInfo->deferred, result);
        napi_delete_async_work(env, stdOutInfo->worker);
        delete stdOutInfo;
    }

    void ChildProcess::ReadStdErr(napi_env env, void *data)
    {
        auto stdErrInfo = (StdInfo*)data;
        char childStderr[MAXSIZE] = {0};
        while (*(stdErrInfo->isNeedRun)) {
            read(stdErrInfo->fd, childStderr, sizeof(childStderr) - 1);
            if (strlen(childStderr) > 0) {
                stdErrInfo->stdData += childStderr;
            }
            if (stdErrInfo->stdData.size() > stdErrInfo->maxBuffSize && *(stdErrInfo->isNeedRun)) {
                if (!kill(stdErrInfo->pid, SIGKILL)) {
                    *(stdErrInfo->isNeedRun) = false;
                    stdErrInfo->stdData = stdErrInfo->stdData.substr(0, stdErrInfo->maxBuffSize);
                } else {
                    HILOG_ERROR("stdErr maxBuff kill signal failed");
                }
            }
            if (memset_s(childStderr, MAXSIZE, '\0', MAXSIZE - 1) != 0) {
                HILOG_ERROR("getOutput memset_s failed");
                return;
            }
        }
    }

    void ChildProcess::EndStdErr(napi_env env, napi_status status, void *buffer)
    {
        auto stdErrInfo = (StdInfo*)buffer;
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = stdErrInfo->stdData.size() + 1;
        napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
        if (memcpy_s(data, bufferSize, (const void*)stdErrInfo->stdData.c_str(),
            stdErrInfo->stdData.size()) != 0) {
            HILOG_ERROR("getErrOutput memcpy_s failed");
            napi_delete_async_work(env, stdErrInfo->worker);
            return;
        }

        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
        napi_resolve_deferred(env, stdErrInfo->deferred, result);
        napi_delete_async_work(env, stdErrInfo->worker);
        delete stdErrInfo;
    }

    int ChildProcess::GetValidSignal(const napi_value signo)
    {
        int32_t sig = 0;
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env_, signo, &valuetype);
        if (valuetype == napi_valuetype::napi_number) {
            napi_get_value_int32(env_, signo, &sig);
            return sig;
        } else if (valuetype == napi_valuetype::napi_string) {
            std::string buffer = RequireStrValue(signo);
            auto iter = g_signalsMap.find(buffer);
            if (iter != g_signalsMap.end()) {
                sig = iter->second;
                return sig;
            } else {
                return g_signalsMap["SIGTERM"];
            }
        } else {
            return g_signalsMap["SIGTERM"];
        }
    }

    void ChildProcess::Kill(const napi_value signo)
    {
        size_t signal = GetValidSignal(signo);
        std::vector<int32_t> signalType = {SIGINT, SIGQUIT, SIGKILL, SIGTERM};
        if (!kill(optionsInfo_->pid, signal)) {
            auto res = std::find(signalType.begin(), signalType.end(), static_cast<int32_t>(signal));
            (res != signalType.end()) ? isNeedRun_ = false : 0;
            killed_ = true;
        } else {
            HILOG_ERROR("kill signal failed");
        }
    }

    void ChildProcess::Close()
    {
        int32_t status = 0;
        if (isWait_ && (waitpid(optionsInfo_->pid, &status, WNOHANG) == 0) && isNeedRun_) {
            if (!kill(optionsInfo_->pid, SIGKILL)) {
                waitpid(optionsInfo_->pid, &status, 0);
                isWait_ = false;
                exitCode_ = status;
                isNeedRun_ = false;
            } else {
                HILOG_ERROR("close kill SIGKILL signal failed");
            }
        }
    }

    void ChildProcess::TimeoutListener(napi_env env, void *data)
    {
        std::vector<int32_t> signalType = {SIGINT, SIGQUIT, SIGKILL, SIGTERM};
        auto temp = (OptionsInfo*)data;
        int32_t timeout = temp->timeout * TIME_EXCHANGE;
        if (timeout > 0) {
            usleep(timeout);
            if (*(temp->isNeedRun)) {
                if (!kill(temp->pid, temp->killSignal)) {
                    auto res = std::find(signalType.begin(), signalType.end(), temp->killSignal);
                    (res != signalType.end()) ? *(temp->isNeedRun) = false : 0;
                } else {
                    HILOG_ERROR("timeout kill signal failed");
                }
            }
        }
    }

    void ChildProcess::InitOptionsInfo(napi_value options)
    {
        std::vector<std::string> keyStr = {"timeout", "killSignal", "maxBuffer"};
        optionsInfo_ = new OptionsInfo();
        size_t size = keyStr.size();
        for (size_t i = 0; i < size; i++) {
            napi_status status = napi_ok;
            napi_value property = nullptr;
            napi_get_named_property(env_, options, keyStr[i].c_str(), &property);
            switch (i) {
                case 0: // 0:timeout
                    status = napi_get_value_int32(env_, property, &optionsInfo_->timeout);
                    if (status != napi_ok) {
                        optionsInfo_->timeout = 0;
                    }
                    break;
                case 1: // 2:killSignal
                    optionsInfo_->killSignal = GetValidSignal(property);
                    break;
                case 2: // 2:maxBuffer
                    status = napi_get_value_int64(env_, property, &optionsInfo_->maxBuffer);
                    if (status != napi_ok) {
                        optionsInfo_->maxBuffer = MAXSIZE * MAXSIZE;
                    }
                    break;
                default:
                    break;
            }
        }
        optionsInfo_->isNeedRun = &isNeedRun_;
    }

    std::string ChildProcess::RequireStrValue(const napi_value strValue)
    {
        char *buffer = nullptr;
        size_t bufferSize = 0;

        napi_get_value_string_utf8(env_, strValue, buffer, -1, &bufferSize);
        if (bufferSize > 0) {
            buffer = new char[bufferSize + 1];
        }

        napi_get_value_string_utf8(env_, strValue, buffer, bufferSize + 1, &bufferSize);

        std::string result;
        if (buffer != nullptr) {
            result = buffer;
        }
        delete []buffer;
        buffer = nullptr;
        return result;
    }

    ChildProcess::~ChildProcess()
    {
        close(stdOutFd_[0]);
        close(stdErrFd_[0]);
        if (isWait_) {
            int32_t status = 0;
            waitpid(optionsInfo_->pid, &status, 0);
        }
        isNeedRun_ = false;
    }
} // namespace

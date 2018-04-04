#pragma once

#include "common.h"
#include "asyncWork.h"

class basePlan
{
  public:
    virtual napi_ref const &getIn() const = 0;
    virtual napi_ref const &getOut() const = 0;
    uint32_t const dim;
    uint32_t const *const size;

    inline basePlan(uint32_t dim, uint32_t const *size)
        : dim(dim),
          size(initArray(dim, size))
    {
    }
    virtual ~basePlan();

    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    static napi_value getInput(napi_env env, napi_callback_info info);
    static napi_value getOutput(napi_env env, napi_callback_info info);
    static napi_value getSize(napi_env env, napi_callback_info info);

    static napi_value calcAsync(napi_env env, napi_callback_info info);
    static void calcAsyncWorker(napi_env env, void *data);
    static void calcAsyncCallback(napi_env env, napi_status status, void *data);

    virtual void calc() = 0;
};
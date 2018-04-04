#pragma once

#include "common.h"

class asyncWork
{
  public:
    void *const data;
    napi_ref const callback;
    napi_async_work const w;

    inline asyncWork(napi_env env, void *data, napi_value callback, async_exec exec, async_cb cb)
        : data(data),
          callback(createReference(env, callback)),
          w(createWork(env, exec, cb, data))
    {
    }
    inline asyncWork(napi_env env, void *data, napi_ref callback, async_exec exec, async_cb cb)
        : data(data),
          callback(callback),
          w(createWork(env, exec, cb, data))
    {
    }

    inline napi_async_work createWork(napi_env env, async_exec exec, async_cb cb, void *data)
    {
        napi_async_work ret;
        N_OK(napi_create_async_work(env, nullptr, nullptr, exec, cb, data, &ret));
        return ret;
    }
};

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
          w(createWork(env, exec, cb, this))
    {
    }
    inline asyncWork(napi_env env, void *data, napi_ref callback, async_exec exec, async_cb cb)
        : data(data),
          callback(callback),
          w(createWork(env, exec, cb, this))
    {
    }
};

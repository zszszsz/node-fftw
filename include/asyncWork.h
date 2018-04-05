#pragma once

#include "common.h"

namespace node_fftw
{
class asyncWork
{
public:
  void *const data;
  napi_ref const callback;
  napi_async_work const w;

  inline asyncWork(napi_env env, napi_value hook, char const *name, napi_value callback, void *data, async_exec exec, async_cb cb)
      : data(data),
        callback(createReference(env, callback)),
        w(createWork(env, hook, name, exec, cb, this))
  {
  }
  inline asyncWork(napi_env env, napi_value hook, char const *name, napi_ref callback, void *data, async_exec exec, async_cb cb)
      : data(data),
        callback(callback),
        w(createWork(env, hook, name, exec, cb, this))
  {
  }
};
}
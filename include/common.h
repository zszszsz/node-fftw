#pragma once

#include <node_api.h>
#include <fftw3.h>
#include <assert.h>

#define N_OK(status)             \
  do                             \
  {                              \
    assert((status) == napi_ok); \
  } while (false)

#define NAPI_METHOD(name, func)             \
  {                                         \
    name, 0, func, 0, 0, 0, napi_default, 0 \
  }
#define NAPI_PROPERTY(name, getter, setter)        \
  {                                                \
    name, 0, 0, getter, setter, 0, napi_default, 0 \
  }

typedef napi_async_execute_callback async_exec;
typedef napi_async_complete_callback async_cb;

static inline uint32_t const *initArray(uint32_t length, uint32_t const *data)
{
  uint32_t *ret = new uint32_t[length];
  for (uint32_t i = 0; i < length; i++)
    ret[i] = data[i];
  return ret;
}
static inline double *node_malloc_double(napi_env env, size_t size, napi_value *result)
{
  double *buf;
  napi_value ab;
  N_OK(napi_create_arraybuffer(env, size * sizeof(double), reinterpret_cast<void **>(&buf), &ab));
  N_OK(napi_create_typedarray(env, napi_float64_array, size, ab, 0, result));
  return buf;
}
static inline napi_ref createReference(napi_env env, napi_value v)
{
  napi_ref ret;
  N_OK(napi_create_reference(env, v, 1, &ret));
  return ret;
}
static inline napi_ref initTypedArray(napi_env env, uint32_t size)
{
  napi_value ta;
  node_malloc_double(env, size, &ta);
  return createReference(env, ta);
}
static inline double* getTAData(napi_env env, napi_ref ta)
{
    napi_value arr;
    void *ret;
    N_OK(napi_get_reference_value(env, ta, &arr));
    N_OK(napi_get_typedarray_info(env, arr, nullptr, nullptr, &ret, nullptr, nullptr));
    return static_cast<double *>(ret);
}
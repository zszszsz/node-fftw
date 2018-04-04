#include "basePlan.h"
#include <stdlib.h>
#include <string.h>

void basePlan::Destructor(napi_env env, void *native, void *hint)
{
    N_OK(napi_delete_reference(env, static_cast<basePlan *>(native)->getIn()));
    N_OK(napi_delete_reference(env, static_cast<basePlan *>(native)->getOut()));
    delete static_cast<basePlan *>(native);
}
basePlan::~basePlan()
{
    delete[] size;
}

napi_value basePlan::getInput(napi_env env, napi_callback_info info)
{
    napi_value jsthis;
    basePlan *obj;
    N_OK(napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    N_OK(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&obj)));

    napi_value ret;
    N_OK(napi_get_reference_value(env, obj->getIn(), &ret));
    return ret;
}

napi_value basePlan::getOutput(napi_env env, napi_callback_info info)
{
    napi_value jsthis;
    basePlan *obj;
    N_OK(napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    N_OK(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&obj)));

    napi_value ret;
    N_OK(napi_get_reference_value(env, obj->getOut(), &ret));
    return ret;
}

napi_value basePlan::getSize(napi_env env, napi_callback_info info)
{
    napi_value jsthis;
    basePlan *obj;
    N_OK(napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    N_OK(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&obj)));

    napi_value ret;
    N_OK(napi_create_array(env, &ret));
    for (uint32_t i = 0; i < obj->dim; i++)
    {
        napi_value n;
        N_OK(napi_create_uint32(env, obj->size[i], &n));
        N_OK(napi_set_element(env, ret, i, n));
    }
    return ret;
}

napi_value basePlan::calcAsync(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value cb;
    napi_value jsthis;
    basePlan *obj;
    N_OK(napi_get_cb_info(env, info, &argc, &cb, &jsthis, nullptr));
    N_OK(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&obj)));

    asyncWork *work = new asyncWork(env, reinterpret_cast<void *>(obj), cb, calcAsyncWorker, calcAsyncCallback);
    N_OK(napi_queue_async_work(env, work->w));
    return nullptr;
}

void basePlan::calcAsyncWorker(napi_env env, void *aw)
{
    static_cast<basePlan *>(static_cast<asyncWork *>(aw)->data)->calc();
}

void basePlan::calcAsyncCallback(napi_env env, napi_status status, void *aw)
{
    napi_value callback;
    napi_ref cbref = static_cast<asyncWork *>(aw)->callback;
    N_OK(napi_get_reference_value(env, cbref, &callback));
    N_OK(napi_call_function(env, nullptr, callback, 0, nullptr, nullptr));
    N_OK(napi_delete_async_work(env, static_cast<asyncWork *>(aw)->w));
    N_OK(napi_delete_reference(env, cbref));
    delete static_cast<asyncWork *>(aw);
}
#include "dftPlan.h"
#include <stdlib.h>
#include <string.h>
void dftPlan::Destructor(napi_env env, void *native, void *hint)
{
    dftPlan *obj = static_cast<dftPlan *>(native);
    if (!obj->isInJs)
        return;
    N_OK(napi_delete_reference(env, obj->getIn()));
    N_OK(napi_delete_reference(env, obj->getOut()));
    delete obj;
}
#define ARGC 4
// dftPlan(isReal, size, sign, flags)
napi_value dftPlan::New(napi_env env, napi_callback_info info)
{
    napi_value target;
    napi_value jsthis;
    size_t argc = ARGC;
    napi_value argv[ARGC];

    N_OK(napi_get_new_target(env, info, &target));
    if (target == nullptr)
        return nullptr;

    N_OK(napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr));
    assert(argc == ARGC);

    bool isReal;
    uint32_t dim;
    uint32_t *size;
    int sign;
    uint32_t flags;

    N_OK(napi_get_value_bool(env, argv[0], &isReal));
    N_OK(napi_get_array_length(env, argv[1], &dim));
    N_OK(napi_get_value_int32(env, argv[2], &sign));
    N_OK(napi_get_value_uint32(env, argv[3], &flags));

    // receive size
    size = static_cast<uint32_t *>(malloc(dim * sizeof(uint32_t)));
    for (uint32_t i = 0; i < dim; i++)
    {
        napi_value tmp;
        N_OK(napi_get_element(env, argv[1], i, &tmp));
        N_OK(napi_get_value_uint32(env, tmp, &size[i]));
    }
    dftPlan *obj = new dftPlan(env, isReal, dim, size, sign, flags);
    N_OK(napi_wrap(env, jsthis, reinterpret_cast<void *>(obj), basePlan::Destructor, nullptr, nullptr));

    free(size);
    return jsthis;
}
#undef ARGC

uint32_t dftPlan::calcInSize(uint32_t dim, bool isReal, uint32_t const *size)
{
    uint32_t ret = isReal ? 1 : 2;
    for (uint32_t i = 0; i < dim; i++)
        ret *= size[i];
    return ret;
}
uint32_t dftPlan::calcOutSize(uint32_t dim, bool isReal, uint32_t const *size)
{
    uint32_t ret = 1;
    for (uint32_t i = 0; i < dim - 1; i++)
        ret *= size[i];
    return ret * (size[dim - 1] / 2 + 1);
}
fftw_plan dftPlan::initPlan(bool isReal, uint32_t dim, uint32_t const *size, double *in, double *out, int sign, uint32_t flags)
{
    if (!isReal)
    { // complex-to-complex
        return fftw_plan_dft(dim, reinterpret_cast<const int *>(size),
                             reinterpret_cast<fftw_complex *>(in), reinterpret_cast<fftw_complex *>(out),
                             sign > 0 ? 1 : -1, flags);
    }
    else
    { // real-to-complex
        return fftw_plan_dft_r2c(dim, reinterpret_cast<const int *>(size),
                                 in, reinterpret_cast<fftw_complex *>(out),
                                 flags);
    }
}

dftPlan::~dftPlan()
{
    fftw_destroy_plan(this->plan);
    if (!isInJs)
    {
        fftw_free(in);
        fftw_free(out);
    }
}

napi_value dftPlan::getIsReal(napi_env env, napi_callback_info info)
{
    napi_value jsthis;
    dftPlan *obj;
    N_OK(napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    N_OK(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&obj)));

    napi_value ret;
    N_OK(napi_get_boolean(env, obj->isReal, &ret));
    return ret;
}

napi_ref const &dftPlan::getIn() const { return this->jsIn; }
napi_ref const &dftPlan::getOut() const { return this->jsOut; }

void dftPlan::calc()
{
    fftw_execute(this->plan);
}
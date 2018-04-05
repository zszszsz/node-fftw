#include "node-fftw.h"

namespace node_fftw
{
napi_value Init(napi_env env, napi_value exports)
{
    N_OK(napi_set_named_property(env, exports, "dftPlan", dftPlan::Init(env)));
    N_OK(napi_set_named_property(env, exports, "cztr1dPlan", cztr1dPlan::Init(env)));
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
}
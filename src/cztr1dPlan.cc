#include "cztPlan.h"

void cztr1dPlan::Destructor(napi_env env, void *native, void *hint)
{
    cztr1dPlan *obj = static_cast<cztr1dPlan *>(native);
    if (!obj->isInJs)
        return;
    // DO NOT call dftPlan::Destructor here, they have nothing to do with node.js
    // dftPlan::Destructor(env, obj->sig, nullptr);
    // dftPlan::Destructor(env, obj->core, nullptr);
    // dftPlan::Destructor(env, obj->idft, nullptr);
    N_OK(napi_delete_reference(env, obj->getIn()));
    N_OK(napi_delete_reference(env, obj->getOut()));
    delete obj;
}
#define ARGC 7
// cztr1dPlan(inSize, outSize, rate, start, stop, sign, flags)
napi_value cztr1dPlan::New(napi_env env, napi_callback_info info)
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

    uint32_t inSize;
    uint32_t outSize;
    double rate;
    double start;
    double stop;
    int sign; // decide \omega is exp(-2i...) or exp(2i...)
    uint32_t flags;

    N_OK(napi_get_value_uint32(env, argv[0], &inSize));
    N_OK(napi_get_value_uint32(env, argv[1], &outSize));
    N_OK(napi_get_value_double(env, argv[2], &rate));
    N_OK(napi_get_value_double(env, argv[3], &start));
    N_OK(napi_get_value_double(env, argv[4], &stop));
    N_OK(napi_get_value_int32(env, argv[5], &sign));
    N_OK(napi_get_value_uint32(env, argv[6], &flags));
    // I have no idea why do we need so many arguments here.

    cztr1dPlan *obj = new cztr1dPlan(env, inSize, outSize, rate, start, stop, sign, flags);
    N_OK(napi_wrap(env, jsthis, reinterpret_cast<void *>(obj), cztr1dPlan::Destructor, nullptr, nullptr));

    return jsthis;
}
#undef ARGC
cztr1dPlan::~cztr1dPlan()
{
    delete this->sig;
    delete this->core;
    delete this->idft;
    if (!isInJs)
    {
        fftw_free(in);
        fftw_free(out);
    }
}

napi_ref const &cztr1dPlan::getIn() const { return this->jsIn; }
napi_ref const &cztr1dPlan::getOut() const { return this->jsOut; }

void cztr1dPlan::calc()
{
    // this should be the Bluestein's algorithm, if you find anything wrong here, tell me plz.
    // where w = expi(+/-2pi *(f2 - f1) / (outSize * rate)),
    double omega = 2.0 * M_PI * (stop - start) / rate / (size[1] - 1) * sign;
    // a = expi(2pi * f1 / rate)
    double alpha = 2.0 * M_PI * start / rate;
    uint32_t t = core->size[0];

    // sig is for dft of signal
    for (uint32_t i = 0; i < size[0]; i++)
    {
        double psi = (double)i * alpha + (double)i * i / 2.0 * omega;
        // [0] element holds the real part
        sig->in[2 * i] = cos(psi) * this->in[i];
        // [1] element holds the imaginary part
        sig->in[2 * i + 1] = sin(psi) * this->in[i];
    }
    // core is for dft of the conv core
    for (uint32_t i = 0; i < size[1]; i++)
    {
        core->in[2 * i] = cos((double)i * i / 2 * omega);
        core->in[2 * i + 1] = sin((double)i * i / 2 * omega);
    }
    for (uint32_t i = size[1]; i < t; i++)
    {
        core->in[2 * i] = cos((double)(t - i) * (t - i) / 2 * omega);
        core->in[2 * i + 1] = cos((double)(t - i) * (t - i) / 2 * omega);
    }
    sig->calc();
    core->calc();
    for (uint32_t i = 0; i < t; i++)
    {
        // re[Result] = re[Sig] * re[Core] - im[Sig] * im[Core]
        idft->in[2 * i] = sig->out[2 * i] * core->out[2 * i] -
                          sig->out[2 * i + 1] * core->out[2 * i + 1];
        // im[Result] = im[Sig] * re[Core] + re[Sig] * im[Core]
        idft->in[2 * i + 1] = sig->out[2 * i + 1] * core->out[2 * i] +
                              sig->out[2 * i] * core->out[2 * i + 1];
    }
    idft->calc();
    // copy result
    for (uint32_t i = 0; i < size[1]; i++)
    {
        double psi = (double)i * i / 2.0 / omega;
        double c = cos(psi);
        double s = sin(psi);
        this->out[2 * i] = c * idft->out[2 * i] - s * idft->out[2 * i + 1];
        this->out[2 * i + 1] = s * idft->out[2 * i] + c * idft->out[2 * i + 1];
    }
    return;
}
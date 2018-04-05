#include "cztPlan.h"

namespace node_fftw
{
void cztr1dPlan::Destructor(napi_env env, void *native, void *hint)
{
    cztr1dPlan *obj = static_cast<cztr1dPlan *>(native);
    if (!obj->isInJs)
        return;
    // DO NOT call dftPlan::Destructor here, they have nothing to do with node.js
    // dftPlan::Destructor(env, obj->sig, nullptr);
    // dftPlan::Destructor(env, obj->core, nullptr);
    // dftPlan::Destructor(env, obj->idft, nullptr);
    N_OK(napi_delete_reference(env, obj->jsIn));
    N_OK(napi_delete_reference(env, obj->jsOut));
    N_OK(napi_delete_reference(env, obj->jsthis));
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
    N_OK(napi_wrap(env, jsthis, reinterpret_cast<void *>(obj), cztr1dPlan::Destructor, nullptr, const_cast<napi_ref *>(&obj->jsthis)));

    return jsthis;
}
#undef ARGC
cztr1dPlan::~cztr1dPlan()
{
    delete this->sigdft;
    delete this->coredft;
    delete this->idft;
    if (!isInJs)
    {
        fftw_free(in);
        fftw_free(out);
    }
}

napi_ref const &cztr1dPlan::getIn() const { return this->jsIn; }
napi_ref const &cztr1dPlan::getOut() const { return this->jsOut; }
napi_ref const &cztr1dPlan::getJsthis() const { return this->jsthis; }

void cztr1dPlan::calc()
{
    // this should be the Bluestein's algorithm, if you find anything wrong here, tell me plz.
    // where w = expi(+/-2pi *(f2 - f1) / (outSize * rate)),
    double omega = 2.0 * M_PI * (stop - start) / rate / (size[1] - 1) * sign;
    // a = expi(2pi * f1 / rate)
    double alpha = 2.0 * M_PI * start / rate;

    complex<double> *sig = reinterpret_cast<complex<double> *>(sigdft->in);
    complex<double> *core = reinterpret_cast<complex<double> *>(coredft->in);

    // set signal and core values, could someone do some optimization here ?
    for (uint32_t j = 0; j < size[1]; j++)
    {
        core[j].real(cos(j * j / 2.0 * omega));
        core[j].imag(cos(j * j / 2.0 * omega));
    }
    for (uint32_t j = size[1]; j < totalSize - size[0]; j++)
    {
        core[j].real(0);
        core[j].imag(0);
    }
    for (uint32_t j = totalSize - size[0]; j < totalSize; j++)
    {
        core[j].real(cos((totalSize - j) * (totalSize - j) / 2.0 * omega));
        core[j].imag(sin((totalSize - j) * (totalSize - j) / 2.0 * omega));
    }
    // signal here
    for (uint32_t j = 0; j < size[0]; j++)
    {
        double psi = j * alpha + j * j / 2.0 * omega;
        double c = cos(psi);
        double s = -sin(psi);
        sig[j].real(c * in[j]);
        sig[j].imag(s * in[j]);
    }
    for (uint32_t j = size[0]; j < totalSize; j++)
    {
        sig[j].real(0);
        sig[j].imag(0);
    }

    sigdft->calc();
    coredft->calc();
    sig = reinterpret_cast<complex<double> *>(sigdft->out);
    core = reinterpret_cast<complex<double> *>(coredft->out);
    complex<double> *inv = reinterpret_cast<complex<double> *>(idft->in);

    for (uint32_t j = 0; j < totalSize; j++)
    {
        inv[j] = sig[j] * core[j] / (double)totalSize;
    }

    idft->calc();
    inv = reinterpret_cast<complex<double> *>(idft->out);
    complex<double> *out = reinterpret_cast<complex<double> *>(this->out);

    // copy result out
    for (uint32_t j = 0; j < size[1]; j++)
    {
        double psi = j * j / 2.0 * omega;
        double c = cos(psi);
        double s = -sin(psi);
        out[j].real(c * inv[j].real() - s * inv[j].imag());
        out[j].imag(s * inv[j].real() - c * inv[j].imag());
    }
    return;
}
}
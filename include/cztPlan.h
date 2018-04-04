#pragma once

#include "common.h"
#include "basePlan.h"
#include "dftPlan.h"

#define _USE_MATH_DEFINES
#include <math.h>

class cztr1dPlan : basePlan
{
  private:
    double const rate;
    double const start;
    double const stop;
    int const sign;
    napi_ref const jsIn;
    napi_ref const jsOut;
    napi_ref const jsthis;
    dftPlan *const sig;
    dftPlan *const core;
    dftPlan *const idft;

  public:
    bool const isInJs;

    virtual napi_ref const &getIn() const;
    virtual napi_ref const &getOut() const;
    virtual napi_ref const &getJsthis() const;
    double *const in;
    double *const out;

    inline cztr1dPlan(napi_env env, uint32_t inSize, uint32_t outSize, double rate, double start, double stop, int sign, uint32_t flags)
        : basePlan(2, new uint32_t[2]{inSize, outSize}),
          rate(rate), start(start), stop(stop), sign(sign > 0 ? 1 : -1),
          jsIn(initTypedArray(env, size[0])),      // real
          jsOut(initTypedArray(env, size[1] * 2)), //complex
          jsthis(0),
          sig(new dftPlan(false, 1, new uint32_t[1]{inSize}, -1, flags)),
          core(new dftPlan(false, 1, new uint32_t[1]{inSize + outSize}, -1, flags)),
          idft(new dftPlan(false, 1, new uint32_t[1]{inSize + outSize}, +1, flags)),
          isInJs(true),
          in(getTAData(env, jsIn)),
          out(getTAData(env, jsOut))
    // using sign=+1 for inverse fourier transform
    {
    }
    inline cztr1dPlan(uint32_t inSize, uint32_t outSize, double rate, double start, double stop, int sign, uint32_t flags)
        : basePlan(2, new uint32_t[2]{inSize, outSize}),
          rate(rate), start(start), stop(stop), sign(sign > 0 ? 1 : -1),
          jsIn(0),
          jsOut(0),
          jsthis(0),
          sig(new dftPlan(false, 1, new uint32_t[1]{inSize}, -1, flags)),
          core(new dftPlan(false, 1, new uint32_t[1]{inSize + outSize}, -1, flags)),
          idft(new dftPlan(false, 1, new uint32_t[1]{inSize + outSize}, +1, flags)),
          isInJs(false),
          in(fftw_alloc_real(sizeof(double) * inSize)),      // real
          out(fftw_alloc_real(sizeof(double) * outSize * 2)) // complex
    // using sign=+1 for inverse fourier transform
    {
    }
    virtual ~cztr1dPlan();

    static inline napi_value Init(napi_env env)
    {
        napi_property_descriptor desc[] = {
            NAPI_PROPERTY("in", basePlan::getInput, 0),
            NAPI_PROPERTY("out", basePlan::getOutput, 0),
            NAPI_PROPERTY("size", basePlan::getSize, 0),
            NAPI_METHOD("calcAsync", basePlan::calcAsync),
        };

        napi_value ret;
        N_OK(napi_define_class(env, "cztr1dPlan", NAPI_AUTO_LENGTH, cztr1dPlan::New, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &ret));
        return ret;
    }
    static napi_value New(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    virtual void calc();
};
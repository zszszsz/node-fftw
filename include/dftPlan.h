#pragma once

#include "common.h"
#include "basePlan.h"

class dftPlan : public basePlan
{
  private:
    uint32_t calcInSize(uint32_t dim, bool isReal, uint32_t const *size);
    uint32_t calcOutSize(uint32_t dim, bool isReal, uint32_t const *size);
    fftw_plan initPlan(bool isReal, uint32_t dim, uint32_t const *size, double *in, double *out, int sign, uint32_t flags);

    bool const isReal;
    uint32_t const inSize;
    uint32_t const outSize;
    napi_ref const jsIn;
    napi_ref const jsOut;
    napi_ref const jsthis;

  public:
    bool const isInJs;

    virtual napi_ref const &getIn() const;
    virtual napi_ref const &getOut() const;
    virtual napi_ref const &getJsthis() const;
    double *const in;
    double *const out;
    fftw_plan const plan;

    inline dftPlan(napi_env env, bool isReal, uint32_t dim, std::vector<uint32_t> const size, int sign, uint32_t flags)
        : basePlan(dim, size),
          isReal(isReal),
          inSize(calcInSize(dim, isReal, this->size)),
          outSize(calcOutSize(dim, isReal, this->size)),
          jsIn(initTypedArray(env, inSize)),
          jsOut(initTypedArray(env, outSize)),
          jsthis(0),
          isInJs(true),
          in(getTAData(env, jsIn)),
          out(getTAData(env, jsOut)),
          plan(initPlan(isReal, dim, this->size, in, out, sign, flags))
    {
    }
    inline dftPlan(bool isReal, uint32_t dim, std::vector<uint32_t> const size, int sign, uint32_t flags)
        : basePlan(dim, size),
          isReal(isReal),
          inSize(calcInSize(dim, isReal, this->size)),
          outSize(calcOutSize(dim, isReal, this->size)),
          jsIn(0),
          jsOut(0),
          jsthis(0),
          isInJs(false),
          in(fftw_alloc_real(sizeof(double) * inSize)),
          out(fftw_alloc_real(sizeof(double) * outSize)),
          plan(initPlan(isReal, dim, this->size, in, out, sign, flags))
    {
    }
    virtual ~dftPlan();

    static inline napi_value Init(napi_env env)
    {
        napi_property_descriptor desc[] = {
            NAPI_PROPERTY("in", basePlan::getInput, 0),
            NAPI_PROPERTY("out", basePlan::getOutput, 0),
            NAPI_PROPERTY("size", basePlan::getSize, 0),
            NAPI_PROPERTY("isReal", getIsReal, 0),
            NAPI_METHOD("calcAsync", basePlan::calcAsync),
        };

        napi_value ret;
        N_OK(napi_define_class(env, "dftPlan", NAPI_AUTO_LENGTH, dftPlan::New, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &ret));
        return ret;
    }
    static napi_value New(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    static napi_value getIsReal(napi_env env, napi_callback_info info);

    virtual void calc();
};
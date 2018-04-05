"use strict";

const fftw = require("./build/Release/fftw");
fftw.flags = {
    "FFTW_MEASURE": 0,
    "FFTW_DESTROY_INPUT": 1,
    "FFTW_UNALIGNED": 1 << 1,
    "FFTW_CONSERVE_MEMORY": 1 << 2,
    "FFTW_EXHAUSTIVE": 1 << 3,
    "FFTW_PRESERVE_INPUT": 1 << 4,
    "FFTW_PATIENT": 1 << 5,
    "FFTW_ESTIMATE": 1 << 6,
    "FFTW_WISDOM_ONLY": 1 << 21
}



module.exports = fftw;
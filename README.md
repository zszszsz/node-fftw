# node-fftw
This repo is for nodejs port and bindings with fftw, the fft library.

This repo **DOES NOT** contain fftw code, you need to install fftw first.

You can download fftw from http://www.fftw.org/download.html

You can install it following http://www.fftw.org/fftw3_doc/Installation-and-Customization.html

## Functions:
* discrete fourier transform(dft) using fftw 
    * real-to-complex, arbitrary dimensions.
    * complex-to-complex, arbitrary dimensions.


* inverse dft using fftw, using dft with sign=+1
* chirp z transform, accept only 1d real number, using [Bluestein's algorithm](https://en.wikipedia.org/wiki/Chirp_Z-transform).

## Examples
* dft and idft : **`new fftw.dftPlan(isReal, size, sign, flags)`**
    * isReal: indicate real number or complex number. The result is always complex,
    * size : an array, the rank of the array indicate the dimension, 
    the values indicate length of each dimension,
    * sign: -1 for dft, +1 for idft,
    * flags: the flags passed to fftw.

````js
const fftw = require("./index.js");
const dftPlan = new fftw.dftPlan(true, [16], -1, fftw.flags.FFTW_DESTROY_INPUT);
for(var i = 0; i < 16; i++)
{
    dftPlan.in[i] = Math.sin(Math.PI / 4 * i) + Math.cos(Math.PI / 4 * i);
}

dftPlan.calcAsync(function(plan) {
    console.log("dft result", this.out, plan.out);
});
````
* czt : **`new fftw.cztr1dPlan(inSize, outSize, sampleRate, fstart, fstop, sign, flags)`**
    * inSize: the size of input buffer, 1d real
    * outSize: the size of output buffer, 1d complex
    * sampleRate: the sample rate
    * fstart: the frequency to start calculation,
    * fstop: the frequency to stop calculation,
    * sign: the sign of czt omega value,
    * flags: the flags passed to fftw.

````js
const fftw = require("./index.js");
const cztPlan = new fftw.cztr1dPlan(16, 8, 1, 1, 8, -1, fftw.flags.FFTW_DESTROY_INPUT);
for(var i = 0; i < 16; i++)
{
    cztPlan.in[i] = Math.sin(Math.PI / 4 * i) + Math.cos(Math.PI / 4 * i);
}

cztPlan.calcAsync(function(plan){
    console.log("czt result", this.out, plan.out);
});
````
## Supported flags
````js
const fftw = require("./index.js");
const flags = fftw.flags;
````
* FFTW_MEASURE
* FFTW_DESTROY_INPUT
* FFTW_UNALIGNED
* FFTW_CONSERVE_MEMORY
* FFTW_EXHAUSTIVE
* FFTW_PRESERVE_INPUT
* FFTW_PATIENT
* FFTW_ESTIMATE
* FFTW_WISDOM_ONLY


## TODO
* Testing and bug fix
* fftw wisdom interface
* discrete cos transform (DCT)
* DST
* DHT
* fftw multi thread, openmp and MPI interface
* other fftw functionalities
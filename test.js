#! /usr/bin/env node

"use strict";

const fftw = require("./index.js");
const dftPlan = new fftw.dftPlan(true, [16], -1, fftw.flags.FFTW_DESTROY_INPUT);
for(var i = 0; i < 16; i++)
{
    dftPlan.in[i] = Math.sin(Math.PI / 4 * i) + Math.cos(Math.PI / 4 * i);
}

dftPlan.calcAsync(function() {
    console.log("dft result", this.out);
});

const cztPlan = new fftw.cztr1dPlan(16, 8, 1, 1, 8, -1, fftw.flags.FFTW_DESTROY_INPUT);

for(var i = 0; i < 16; i++)
{
    cztPlan.in[i] = Math.sin(Math.PI / 4 * i) + Math.cos(Math.PI / 4 * i);
}

cztPlan.calcAsync(function(){
    console.log("czt result", this.out);
});
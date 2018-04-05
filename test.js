#! /usr/bin/env node

"use strict";

const fftw = require("./index.js");
const dftPlan = new fftw.dftPlan(true, [16], -1, fftw.flags.FFTW_DESTROY_INPUT);
for (var i = 0; i < 16; i++) {
    dftPlan.in[i] = Math.sin(Math.PI / 4 * i) + Math.cos(Math.PI / 4 * i);
}
dftPlan.calcAsync(function () {
    for (var i = 0; i < this.out.length; i += 2) {
        // console.log(this.out[i] + " + " + this.out[i + 1] + "i");
        console.log(this.out[i] * this.out[i] + this.out[i + 1] * this.out[i + 1]);
    }
    console.log();
});

const cztPlan = new fftw.cztr1dPlan(16, 8, 1, 1, 8, +1, fftw.flags.FFTW_DESTROY_INPUT);

for (var i = 0; i < 16; i++) {
    cztPlan.in[i] = Math.sin(Math.PI / 4 * i) + Math.cos(Math.PI / 4 * i);
}

cztPlan.calcAsync(function () {
    for (var i = 0; i < this.out.length; i += 2) {
        // console.log(this.out[i] + " + " + this.out[i + 1] + "i");
        console.log(this.out[i] * this.out[i] + this.out[i + 1] * this.out[i + 1]);
    }
    console.log();
});
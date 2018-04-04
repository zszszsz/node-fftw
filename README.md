# node-fftw
This repo is for nodejs port and bindings with fftw, the fft library.

This repo **DOES NOT** contain fftw code, you need to install fftw first.

You can download fftw from http://www.fftw.org/download.html

You can install it following http://www.fftw.org/fftw3_doc/Installation-and-Customization.html

## Functions:
* discrete FT(dft) using fftw 
    * real-to-complex, arbitrary rank
    * complex-to-complex, arbitrary rank
* inverse dft using fftw, using dft with sign=+1
* chirp z transform, using 

## Supported flags
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
* discrete cos transform (DCT)
* DST
* DHT
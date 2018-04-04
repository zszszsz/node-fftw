{
    "targets": [
        {
            "target_name": "fftw",
            "include_dirs":["./include"],
            "sources": [
                "src/exports.cc",
                "src/basePlan.cc",
                "src/dftPlan.cc",
                "src/cztr1dPlan.cc",
            ],
            "libraries": [
                "-lfftw3",
                "-lm"
            ]
        }
    ]
}
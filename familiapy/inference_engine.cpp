#include <pybind11/pybind11.h>

#include "familia/inference_engine.h"
// #ifndef FAMILIA_INFERENCE_ENGINE_H
// #define FAMILIA_INFERENCE_ENGINE_H

// #include <memory>

// #include "familia/util.h"
// #include "familia/config.pb.h"
// #include "familia/vocab.h"
// #include "familia/model.h"
// #include "familia/sampler.h"
// #include "familia/document.h"

namespace py = pybind11;

// using namespace  familia;
// /usr/local/opt/gcc@6/lib/gcc/6/libstdc++.6.dylib 
// from familiapy.inference_engine import InferenceEngine
PYBIND11_MODULE(inference_engine, m) {
  py::class_<familia::InferenceEngine>(m, "InferenceEngine")
        .def(py::init<const std::string &,const std::string&,familia::SamplerType >())
        .def("infer",(int (familia::InferenceEngine::*)(const std::vector<std::string>& , familia::LDADoc& )) &familia::InferenceEngine::infer)
        .def("infer",(int (familia::InferenceEngine::*)(const std::vector<std::vector<std::string>>& , familia::SLDADoc& )) &familia::InferenceEngine::infer)
        .def("lda_infer", &familia::InferenceEngine::lda_infer)
        .def("slda_infer", &familia::InferenceEngine::slda_infer)
        ;
}
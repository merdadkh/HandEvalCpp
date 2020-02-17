
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include "ProbEval.h"

size_t Str2Index(std::string hand) 
{
    unsigned long long hand1Mask = CHandEval::ParseHand(hand);
    return CHandIterator::PocketMask2IndexMap.find(hand1Mask)->second;
}

py::array_t<double> ComputeFlopProb(std::string flop) 
{
	unsigned long long flopMask = CHandEval::ParseHand(flop);
	double* prob = CProbEval::LoadFlopProb_fromFile(flopMask);
    return py::array_t<double>({POCKET_HAND_COUNT, POCKET_HAND_COUNT}, prob);
}

py::array_t<double> ComputePreFlopProb() 
{
	double* prob = CProbEval::LoadPreFlopProb_fromFile();
    return py::array_t<double>({POCKET_HAND_COUNT, POCKET_HAND_COUNT}, prob);
}

PYBIND11_MODULE(PokerProbEval, m) 
{
    // optional module docstring
    m.doc() = "computing poker probabilty matrix";
    // define add function
    m.def("Str2Index", &Str2Index, "Convert Pocket str to index of probability matrix");
    m.def("ComputeFlopProb", &ComputeFlopProb, "A function to compute pre-Flop probabilty matrix");
    m.def("ComputePreFlopProb", &ComputePreFlopProb, "A function to compute post-Flop probabilty matrix");
}


#include "qubit.h"

#include <cmath>
#include <complex>

using namespace std::complex_literals;


const simulator::Qubit simulator::Qubit::zero = Qubit(1. + 0i, 0i);
const simulator::Qubit simulator::Qubit::one  = Qubit(0i, 1. + 0i);


simulator::Qubit::Qubit (arma::cx_double alpha, arma::cx_double beta) {
    double abs = std::sqrt(std::pow(std::abs(alpha), 2) + std::pow(std::abs(beta), 2));
    arma::cx_double _alpha = alpha/abs;
    arma::cx_double _beta  = beta/abs;
    
    this->_qubit = { _alpha, _beta };
}

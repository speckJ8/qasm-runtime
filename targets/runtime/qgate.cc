#include "qgate.h"

#include <complex>
#include <cmath>


using namespace std::complex_literals;

const arma::cx_mat runtime::Qgate::_I = { { 1. + 0i, 0i }, { 0i, 1. + 0i } };
const arma::cx_mat runtime::Qgate::_X = { { 0i, 1. + 0i }, { -1. + 0i, 0i } };
const arma::cx_mat runtime::Qgate::_Y = { { 0i, -1i }, { 1i, 0i } };
const arma::cx_mat runtime::Qgate::_Z = arma::cx_mat({ { 1. + 0i, 0i }, { 0i,-1. + 0i } });

const runtime::Qgate runtime::Qgate::I = runtime::Qgate(runtime::Qgate::_I);
const runtime::Qgate runtime::Qgate::X = runtime::Qgate(runtime::Qgate::_X);
const runtime::Qgate runtime::Qgate::Y = runtime::Qgate(runtime::Qgate::_Y);
const runtime::Qgate runtime::Qgate::Z = runtime::Qgate(runtime::Qgate::_Z);


runtime::Qgate::Qgate (double theta, double phi, double lambda) {
    auto ct2 = std::cos(theta/2);
    auto st2 = std::sin(theta/2);
    auto eippl = std::exp(((lambda + phi)/2)*1i);
    auto eipml = std::exp(((lambda - phi)/2)*1i);

    this->unitary = arma::cx_mat({
        { std::conj(ct2 * eippl), -1 * st2 * eipml },
        { std::conj(st2 * eipml),      ct2 * eipml },
    });
}

runtime::Qgate::Qgate (arma::cx_mat unitary) {
    // TODO: verify that `unitary` is a unitary matrix
    this->unitary = unitary;
}

/*runtime::Qubit runtime::Qgate::apply (runtime::Qubit& qubit) {
    arma::cx_vec res = this->unitary*qubit._qubit;
    auto new_qubit = Qubit(res[0], res[1]);
    return new_qubit;
}

runtime::Qubit runtime::Qgate::operator () (runtime::Qubit& qubit) {
    return this->apply(qubit);
}*/

runtime::Qgate runtime::Qgate::operator * (runtime::Qgate& q) {
    auto m = this->unitary*q.unitary;
    return Qgate(m);
}

bool runtime::Qgate::operator == (runtime::Qgate& q) {
    return arma::approx_equal(this->unitary, q.unitary, "absdiff", 0.001);
}

#include "qgate.h"

#include <complex>
#include <cmath>


using namespace std::complex_literals;

const arma::cx_mat simulator::Qgate::_I = { { 1. + 0i, 0i }, { 0i, 1. + 0i } };
const arma::cx_mat simulator::Qgate::_X = { { 0i, 1. + 0i }, { -1. + 0i, 0i } };
const arma::cx_mat simulator::Qgate::_Y = { { 0i, -1i }, { 1i, 0i } };
const arma::cx_mat simulator::Qgate::_Z = arma::cx_mat({ { 1. + 0i, 0i }, { 0i,-1. + 0i } });

const simulator::Qgate simulator::Qgate::I = simulator::Qgate(simulator::Qgate::_I);
const simulator::Qgate simulator::Qgate::X = simulator::Qgate(simulator::Qgate::_X);
const simulator::Qgate simulator::Qgate::Y = simulator::Qgate(simulator::Qgate::_Y);
const simulator::Qgate simulator::Qgate::Z = simulator::Qgate(simulator::Qgate::_Z);


simulator::Qgate::Qgate (double theta, double phi, double lambda) {
    auto ct2 = std::cos(theta/2);
    auto st2 = std::sin(theta/2);
    auto eippl = std::exp(((lambda + phi)/2)*1i);
    auto eipml = std::exp(((lambda - phi)/2)*1i);

    this->unitary = arma::cx_mat({
        { std::conj(ct2 * eippl), -1 * st2 * eipml },
        { std::conj(st2 * eipml),      ct2 * eipml },
    });
}

simulator::Qgate::Qgate (arma::cx_mat unitary) {
    // TODO: verify that `unitary` is a unitary matrix
    this->unitary = unitary;
}

simulator::Qubit simulator::Qgate::apply (simulator::Qubit& qubit) {
    arma::cx_vec res = this->unitary*qubit._qubit;
    auto new_qubit = Qubit(res[0], res[1]);
    return new_qubit;
}

simulator::Qubit simulator::Qgate::operator () (simulator::Qubit& qubit) {
    return this->apply(qubit);
}

simulator::Qgate simulator::Qgate::operator * (simulator::Qgate& q) {
    auto m = this->unitary*q.unitary;
    return Qgate(m);
}

bool simulator::Qgate::operator == (simulator::Qgate& q) {
    return arma::approx_equal(this->unitary, q.unitary, "absdiff", 0.001);
}

#pragma once


#include <armadillo>

#include "qubit.h"


namespace simulator{

class Qubit;

/**
 * Single qubit quantum gate
 * */
class Qgate {

public:

    /**
     * Pauli matrices
     * */
    static const Qgate I, X, Y, Z;

    /**
     * Create a quantum gate defined by the unitary matrix U(theta, phi, lambda)
     * */
    Qgate (double theta, double phi, double lambda);

    /**
     * Apply the gate to a qubit
     * */
    Qubit apply (Qubit& qubit);
    Qubit operator () (Qubit& qubit);

    Qgate operator * (Qgate& q);
    bool operator == (Qgate& q);
private:

    /**
     * Pauli matrices
     * */
    static const arma::cx_mat _I, _X, _Y, _Z;

    /**
     * Make from unitary matrix
     * */
    Qgate (arma::cx_mat);

    /**
     * 2x2 unitary underlying the gate
     * */
    arma::cx_mat unitary;

};

};

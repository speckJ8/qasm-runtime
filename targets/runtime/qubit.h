#ifndef __RUNTIME__QUBIT_H__
#define __RUNTIME__QUBIT_H__

#include <armadillo>

#include "qgate.h"

namespace runtime {

/**
 * Quantum bit
 * */
class Qubit {

public:

    /**
     * Computational basis qubits
     * */
    static const Qubit zero, one;

    /**
     * Normalizes (alpha, beta) before initializing the qubit 
     * */
    Qubit (arma::cx_double alpha, arma::cx_double beta);

    //bool operator == (Qubit a, Qubit b);
    //arma::cx_double dot (Qubit b);
    //runtime::Qgate outer (Qubit b);

private:

    friend class Qgate;

    /*
     * The 2d complex vector that the qubit represents
     */
    arma::cx_vec _qubit;

};

};


#endif // __RUNTIME__QUBIT_H__

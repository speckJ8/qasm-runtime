#pragma once


#include <armadillo>

#include "qgate.h"


namespace simulator {

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
    //simulator::Qgate outer (Qubit b);

private:

    friend class Qgate;

    /*
     * The 2d complex vector that the qubit represents
     */
    arma::cx_vec _qubit;

};

};

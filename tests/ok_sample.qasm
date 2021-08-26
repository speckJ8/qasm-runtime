OPENQASM 2.0;
qreg qubit[2];
gate somegate (a, b) x, y {}
gate cx c,t { CX c,t; }
gate id a { U(0,0,0) a; }
gate x a { u3(pi,0,pi) a; }
gate y a { u3(pi,pi/2,pi/2) a; }
gate z a { u1(pi) a; }
gate h a { u2(0,pi) a; }
gate s a { u1(pi/2) a; }
gate sdg a { u1(-pi/2) a; }
gate t a { u1(pi/4) a; }
gate tdg a { u1(-pi/4) a; }
gate rx(theta) a { u3(theta,-pi/2,pi/2) a; }
gate ry(theta) a { u3(theta,0,0) a; }
gate rz(phi) a { u1(phi) a; }
qreg qq[12];
creg cc[9];
measure qq -> cc;

gate u1 (a)       x { U(a, 0, 0) x; }
gate u2 (a, b)    y { U(a, b, 0) y; }
gate u3 (a, b, c) z { U(a, b, c) z; }

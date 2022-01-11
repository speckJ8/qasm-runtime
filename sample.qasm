OPENQASM 2.0;
qreg qq[2];
creg cc[2];
gate cx c,t { CX c,t; }
# this is some comment... 
gate x a {
    u3(pi,2/3,0) a;
}
gate y a { U(0,0,2*3^4.0) a; }
# this is another comment
# test, test, test
x qq[0];
y qq[1];
barrier qq;
measure qq -> cc;
reset qq;

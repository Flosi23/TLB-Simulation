/**
 * This class only exists to verify that compiling and executing system_c code works;
 */
#ifndef TRACEFIX_HPP
#define TRACEFIX_HPP

#include <systemc>

using namespace sc_core;

SC_MODULE(HELPER) {

    sc_in<int> a;
    sc_in<int> b;
    sc_out<int> out;

    SC_CTOR(HELPER) {
        SC_THREAD(update);
        sensitive << a << b;
    }

    void update() {
        while (true) {
            wait();
            if (b->read() == 0) {
                continue;
            }

            int a_temp = a->read();
            int b_temp = b->read();
            while (a_temp >= b_temp) {
                a_temp = a_temp - b_temp;
            }
            out->write(a_temp);
        }
    }
};

SC_MODULE(SEARCHER) {

    sc_in<bool> clk;
    sc_in<int> a;
    sc_in<int> b;

    sc_out<int> out_a;
    sc_out<int> out_b;
    sc_out<bool> done;

    HELPER helper;
    sc_signal<int> helper_result;

    SC_CTOR(SEARCHER) : helper("helper") {
        helper.a(out_a);
        helper.b(out_b);
        helper.out(helper_result);

        SC_THREAD(update);
        sensitive << clk.pos();
    }

    void update() {
        done->write(false);
        out_a->write(a->read());
        out_b->write(b->read());

        while (true) {
            wait();
            if (out_b == 0) {
                done->write(true);
                continue;
            }
            wait(SC_ZERO_TIME);
            int temp = helper_result.read();
            out_a->write(out_b->read());
            out_b->write(temp);
        }
    }
};


#endif
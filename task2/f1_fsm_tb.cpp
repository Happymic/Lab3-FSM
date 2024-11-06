#include "Vf1_fsm.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

#define MAX_SIM_CYC 1000000

int main(int argc, char **argv) {
    int simcyc;     // simulation clock count
    int tick;       // each clk cycle has two ticks for two edges
    
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // Create an instance of the module
    Vf1_fsm *top = new Vf1_fsm;
    
    // Initialize trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("f1_fsm.vcd");

    // Initialize Vbuddy
    if (vbdOpen()!=1) return(-1);
    vbdHeader("F1 FSM");
    vbdSetMode(1);  // Set flag mode to one-shot
    
    // Initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0;
    
    // Run simulation for MAX_SIM_CYC clock cycles
    for (simcyc=0; simcyc<MAX_SIM_CYC; simcyc++) {
        // Dump variables into VCD file and toggle clock
        for (tick=0; tick<2; tick++) {
            tfp->dump (2*simcyc+tick);
            top->clk = !top->clk;
            top->eval ();
        }

        // Enable controlled by Vbuddy flag
        top->en = vbdFlag();
        
        // Reset for first 2 cycles
        if (simcyc < 2) 
            top->rst = 1;
        else
            top->rst = 0;
            
        // Update Vbuddy display
        vbdBar(top->data_out & 0xFF);
        vbdCycle(simcyc);

        if (vbdGetkey()=='q') 
            break;
    }

    vbdClose();     // clean up
    tfp->close();   
    delete top;
    delete tfp;
    exit(0);
}
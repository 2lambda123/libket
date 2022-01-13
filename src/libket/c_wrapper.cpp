/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/process.hpp>
#include <ket/libket.h>
#include <boost/exception/diagnostic_information.hpp> 
#include <stdarg.h>

using namespace ket::libket;

std::string ket_error_str;

template<class F>
int ket_error_wrapper(F func) {
    try {
        func();
        return KER_SUCCESS;
    } catch (...)  {
        ket_error_str = boost::current_exception_diagnostic_information();
        return KER_ERROR;
    }
}

#define unpack_args(T, args)\
va_list valist;\
va_start(valist, num);\
std::vector<T> args;\
for (int i = 0; i < num; i++) {\
    args.push_back(va_arg(valist, T));\
}

const char* ket_error_message() {
    return ket_error_str.c_str();
}

int ket_process_new(ket_process_t* process, unsigned pid) {
    return ket_error_wrapper([&](){
        *process = (ket_process_t*) new process_t{pid};
    });
}

int ket_process_delete(ket_process_t process) {
    return ket_error_wrapper([&](){
        delete (process_t*) process;
    }); 
}

int ket_process_alloc(ket_process_t process, ket_qubit_t qubit, _Bool dirty) {
    return ket_error_wrapper([&](){
        *((qubit_t*)qubit) = ((process_t*)process)->alloc(dirty);
    });
}

int ket_process_free(ket_process_t process, ket_qubit_t qubit, _Bool dirty) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->free(*((qubit_t*)qubit), dirty);
    });
}

int ket_process_gate(ket_process_t process, ket_gate_t _gate, ket_qubit_t qubit, double param) {
    gate_t gate;
    switch (_gate) {
    case KET_PAULI_X:
        gate = gate_t::pauli_x;
        break;
    case KET_PAULI_Y:
        gate = gate_t::pauli_y;
        break;
    case KET_PAULI_Z:
        gate = gate_t::pauli_z;
        break;
    case KET_ROTATION_X:
        gate = gate_t::rotation_x;
        break;
    case KET_ROTATION_Y:
        gate = gate_t::rotation_y;
        break;
    case KET_ROTATION_Z:
        gate = gate_t::rotation_z;
        break;
    case KET_HADAMARD:
        gate = gate_t::hadamard;
        break;
    case KET_PHASE:
        gate = gate_t::phase;
        break;
    default:
        ket_error_str = "undefined quantum gate";
        return KER_ERROR;
    }
    return ket_error_wrapper([&](){
        ((process_t*)process)->gate(gate, *((qubit_t*)qubit), param);
    });
}

int ket_process_measure(ket_process_t process, ket_future_t future, int num, ...) {
    unpack_args(ket_qubit_t, qubits); 
    return ket_error_wrapper([&](){
        qubit_list_t qubit_list;
        for (int i = 0; i < num; i++) {
            qubit_list.push_back(*((qubit_t*)qubits[i]));
        }
        *((future_t*)future) = ((process_t*)process)->measure(qubit_list);
    });
}

int ket_process_ctrl_push(ket_process_t process, int num, ...) {
    unpack_args(ket_qubit_t, qubits); 
    return ket_error_wrapper([&](){
        qubit_list_t qubit_list;
        for (int i = 0; i < num; i++) {
            qubit_list.push_back(*((qubit_t*)qubits[i]));
        }
        ((process_t*)process)->ctrl_push(qubit_list);
    });
}

int ket_process_ctrl_pop(ket_process_t process) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->ctrl_pop();
    }); 
}

int ket_process_dump(ket_process_t process, ket_dump_t dump, int num, ...) {
    unpack_args(ket_qubit_t, qubits); 
    return ket_error_wrapper([&](){
        qubit_list_t qubit_list;
        for (int i = 0; i < num; i++) {
            qubit_list.push_back(*((qubit_t*)qubits[i]));
        }
        *((dump_t*)dump) = ((process_t*)process)->dump(qubit_list);
    });
}

int ket_qubit_new(ket_qubit_t* qubit) {
    return ket_error_wrapper([&](){
        *qubit = (ket_qubit_t*) new qubit_t{};
    });
}

int ket_qubit_delete(ket_qubit_t qubit) {
    return ket_error_wrapper([&](){
        delete (qubit_t*) qubit;
    }); 
}

int ket_future_new(ket_future_t* future) {
    return ket_error_wrapper([&](){
        *future = (ket_future_t*) new future_t{};
    });
}

int ket_future_delete(ket_future_t future) {
    return ket_error_wrapper([&](){
        delete (future_t*) future;
    }); 
}

int ket_future_value(ket_future_t future, long* value) {
    return ket_error_wrapper([&](){
        *value = ((future_t*)future)->value();
    }); 
}

int ket_future_set(ket_future_t future, ket_future_t value) {
    return ket_error_wrapper([&](){
        ((future_t*)future)->set(*((future_t*)value));
    }); 
}


int ket_label_new(ket_label_t* label) {
    return ket_error_wrapper([&](){
        *label = (ket_label_t*) new label_t{};
    });
}

int ket_label_delete(ket_label_t label) {
    return ket_error_wrapper([&](){
        delete (label_t*) label;
    }); 
}

int ket_dump_new(ket_dump_t* dump) {
    return ket_error_wrapper([&](){
        *dump = (ket_dump_t*) new dump_t{};
    });
}

int ket_dump_delete(ket_dump_t dump) {
    return ket_error_wrapper([&](){
        delete (dump_t*) dump;
    }); 
}

int ket_dump_states(ket_dump_t dump, ket_dump_states_t* states, size_t* size) {
    return ket_error_wrapper([&](){
        *states =  (ket_dump_states_t) &((dump_t*)dump)->states();
        *size = ((dump_t*)dump)->states().size();
    }); 
}

int ket_dump_amplitides(ket_dump_t dump, ket_dump_amplitides_t* amplitides, size_t* size) {
    return ket_error_wrapper([&](){
        *amplitides =  (ket_dump_amplitides_t) &((dump_t*)dump)->amplitides();
        *size = ((dump_t*)dump)->amplitides().size();
    }); 
}

int ket_dump_state_at(ket_dump_states_t states, ket_dump_state_t* state, size_t* size, unsigned long index) {
    return ket_error_wrapper([&](){
        *state = ((ket::dump::states_t*)states)->at(index).data();
        *size = ((ket::dump::states_t*)states)->at(index).size();
    });
}

int ket_dump_amp_at(ket_dump_amplitides_t amplitides, ket_dump_amplitide_t* amplitide, size_t* size, unsigned long index) {
    return ket_error_wrapper([&](){
        *amplitide = (ket_dump_amplitide_t*) &((ket::dump::amplitudes_t*)amplitides)->at(index);
        *size = ((ket::dump::amplitudes_t*)amplitides)->at(index).size();
    }); 
}

int ket_dump_amp(ket_dump_amplitide_t amplitide, double* real, double* imag, unsigned long index) {
    return ket_error_wrapper([&](){
        auto amp = ((ket::dump::prob_amp_t*)amplitide)->at(index);
        *real = amp.real();
        *imag = amp.imag();
    });
}

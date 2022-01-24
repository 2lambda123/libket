/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <boost/exception/diagnostic_information.hpp> 
#include <ket/libket.h>
#include <ket/libket/future_op.hpp>
#include <ket/libket/process.hpp>
#include <stdarg.h>

using namespace ket::libket;

std::string ket_error_str;

template<class F>
int ket_error_wrapper(F func) {
    try {
        func();
        return KET_SUCCESS;
    } catch (...)  {
        ket_error_str = boost::current_exception_diagnostic_information();
        return KET_ERROR;
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
        return KET_ERROR;
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

int ket_process_new_int(ket_process_t process, ket_future_t future, long value) {
    return ket_error_wrapper([&](){
        *((future_t*)future) = ((process_t*)process)->new_int(value);
    });
}

int ket_process_plugin(ket_process_t process, char* name, char* args, int num, ...) {
    unpack_args(ket_qubit_t, qubits); 
    return ket_error_wrapper([&](){
        qubit_list_t qubit_list;
        for (int i = 0; i < num; i++) {
            qubit_list.push_back(*((qubit_t*)qubits[i]));
        }
        ((process_t*)process)->plugin(name, qubit_list, args);
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

int ket_process_adj_begin(ket_process_t process) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->adj_begin();
    }); 
}

int ket_process_adj_end(ket_process_t process) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->adj_end();
    });
}

int ket_process_get_label(ket_process_t process, ket_label_t label) {
    return ket_error_wrapper([&](){
        *((label_t*)label) = ((process_t*)process)->get_label();
    });
}

int ket_process_open_block(ket_process_t process, ket_label_t label) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->open_block(*((label_t*)label));
    });
}

int ket_process_jump(ket_process_t process, ket_label_t label) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->jump(*((label_t*)label));
    });
}

int ket_process_branch(ket_process_t process, ket_future_t future, ket_label_t then, ket_label_t otherwise) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->branch(*((future_t*)future), *((label_t*)then), *((label_t*)otherwise));
    });
}

int ket_process_run(ket_process_t process) {
    return ket_error_wrapper([&](){
        ((process_t*)process)->run();
    });
}

int ket_process_exec_time(ket_process_t process, double* exec_time) {
    return ket_error_wrapper([&](){
        *exec_time = ((process_t*)process)->exec_time();
    }); 
}

int ket_process_id(ket_process_t process, unsigned* pid) {
    return ket_error_wrapper([&](){
        *pid = ((process_t*)process)->process_id();
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

int ket_qubit_index(ket_qubit_t qubit, unsigned* index) {
    return ket_error_wrapper([&](){
        *index = ((qubit_t*)qubit)->index();
    }); 
}

int ket_qubit_measured(ket_qubit_t qubit, _Bool* measured) {
    return ket_error_wrapper([&](){
        *measured = ((qubit_t*)qubit)->measured();
    }); 
}

int ket_qubit_allocated(ket_qubit_t qubit, _Bool* allocated) {
    return ket_error_wrapper([&](){
        *allocated = ((qubit_t*)qubit)->allocated();
    }); 
}

int ket_qubit_process_id(ket_qubit_t qubit, unsigned* pid) {
    return ket_error_wrapper([&](){
        *pid = ((qubit_t*)qubit)->process_id();
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

int ket_future_available(ket_future_t future, _Bool* available) {
    return ket_error_wrapper([&](){
        *available = ((future_t*)future)->available();
    });
}

int ket_future_index(ket_future_t future, unsigned* index) {
    return ket_error_wrapper([&](){
        *index = ((future_t*)future)->index();
    });
}

int ket_future_process_id(ket_future_t future, unsigned* pid) {
    return ket_error_wrapper([&](){
        *pid = ((future_t*)future)->process_id();
    });
}

template< class L, class R, class c_L, class c_R>
int ket_future_op(ket_future_t result, ket_int_op_t int_op, c_L left, c_R right) {
    return ket_error_wrapper([&](){
        switch (int_op) {
        case KET_INT_EQ:
            *((future_t*)result) = *((L*)left) == *((R*)right);
            break;
        case KET_INT_NEQ:
            *((future_t*)result) = *((L*)left) != *((R*)right);
            break;
        case KET_INT_GT:
            *((future_t*)result) = *((L*)left) > *((R*)right);
            break;
        case KET_INT_GEQ:
            *((future_t*)result) = *((L*)left) >= *((R*)right);
            break;
        case KET_INT_LT:
            *((future_t*)result) = *((L*)left) < *((R*)right);
            break;
        case KET_INT_LEQ:
            *((future_t*)result) = *((L*)left) <= *((R*)right);
            break;
        case KET_INT_ADD:
            *((future_t*)result) = *((L*)left) + *((R*)right);
            break;
        case KET_INT_SUB:
            *((future_t*)result) = *((L*)left) - *((R*)right);
            break;
        case KET_INT_MUL:
            *((future_t*)result) = *((L*)left) * *((R*)right);
            break;
        case KET_INT_DIV:
            *((future_t*)result) = *((L*)left) / *((R*)right);
            break;
        case KET_INT_SLL:
            *((future_t*)result) = *((L*)left) << *((R*)right);
            break;
        case KET_INT_SRL:
            *((future_t*)result) = *((L*)left) >> *((R*)right);
            break;
        case KET_INT_AND:
            *((future_t*)result) = *((L*)left) & *((R*)right);
            break;
        case KET_INT_OR:
            *((future_t*)result) = *((L*)left) | *((R*)right);
            break;
        case KET_INT_XOR:
            *((future_t*)result) = *((L*)left) ^ *((R*)right);
            break;
        default:
            throw std::runtime_error{"undefined int (future_t) operation"};
        }
    });
}

int ket_future_op(ket_future_t result, ket_int_op_t int_op, ket_int_op_tt_t tt, ...) {
    ket_future_t fl, fr;
    long il, ir;
    va_list valist;
    va_start(valist, tt);

    switch (tt) {
    case KET_INT_FF:
        fl =  va_arg(valist, ket_future_t);
        fr =  va_arg(valist, ket_future_t);
        return ket_future_op<future_t, future_t>(result, int_op, fl, fr);
        break;
    case KET_INT_FI:
        fl =  va_arg(valist, ket_future_t);
        ir =  va_arg(valist, long);
        return ket_future_op<future_t, long>(result, int_op, fl, &ir);
    case KET_INT_IF:
        il =  va_arg(valist, long);
        fr =  va_arg(valist, ket_future_t);
        return ket_future_op<long, future_t>(result, int_op, &il, fr);
    default:
        ket_error_str = "ket_future_op (libketc) invalid arguments";
        return KET_ERROR;
    }
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

int ket_label_index(ket_label_t label, unsigned* index) {
    return ket_error_wrapper([&](){
        *index = ((label_t*)label)->index();
    });
}

int ket_label_process_id(ket_label_t label, unsigned* pid) {
    return ket_error_wrapper([&](){
        *pid = ((label_t*)label)->process_id();
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

int ket_dump_amplitudes(ket_dump_t dump, ket_dump_amplitudes_t* amplitudes, size_t* size) {
    return ket_error_wrapper([&](){
        *amplitudes =  (ket_dump_amplitudes_t) &((dump_t*)dump)->amplitudes();
        *size = ((dump_t*)dump)->amplitudes().size();
    }); 
}

int ket_dump_available(ket_dump_t dump, _Bool* available) {
    return ket_error_wrapper([&](){
        *available =  ((dump_t*)dump)->available();        
    }); 

}

int ket_dump_index(ket_dump_t dump, unsigned* index) {
    return ket_error_wrapper([&](){
        *index =  ((dump_t*)dump)->index();
    }); 

}

int ket_dump_process_id(ket_dump_t dump, unsigned* pid) {
    return ket_error_wrapper([&](){
        *pid =  ((dump_t*)dump)->process_id();
    }); 

}

int ket_dump_state_at(ket_dump_states_t states, ket_dump_state_t* state, size_t* size, unsigned long index) {
    return ket_error_wrapper([&](){
        *state = ((ket::dump::states_t*)states)->at(index).data();
        *size = ((ket::dump::states_t*)states)->at(index).size();
    });
}

int ket_dump_amp_at(ket_dump_amplitudes_t amplitudes, double* real, double* imag, unsigned long index) {
    return ket_error_wrapper([&](){
        *real = ((ket::dump::amplitudes_t*)amplitudes)->at(index).real();
        *imag = ((ket::dump::amplitudes_t*)amplitudes)->at(index).imag();
    }); 
}

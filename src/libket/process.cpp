/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/process.hpp>
#include <ket/libket/process_exception.hpp>
#include <boost/smart_ptr/make_shared.hpp>

using namespace ket::libket;

process_t::process_t(index_t process_id) :
    pid{process_id},
    alive{boost::make_shared<bool>(true)}
{
    block_map[0] = block_t{};
    current_block = 0;
}

process_t::~process_t() {
    *alive = false;
}


qubit_t process_t::alloc(bool dirty) {
    check_not_adj_ctrl();

    block_map[current_block].add_instruction(
        dirty? 
        quantum_code::intruction_t{
            quantum_code::op_code_t::alloc_dirty,
            qubit_count
        } : 
        quantum_code::intruction_t{
            quantum_code::op_code_t::alloc,
            qubit_count
        }
    );

    num_qubit += 1;
    max_num_qubit = num_qubit > max_num_qubit? num_qubit : max_num_qubit;

    if (dirty) add_feature(features_t::qubit_alloc_dirty);

    return qubit_t{qubit_count++, process_id()};
}

void process_t::free(qubit_t qubit, bool dirty) {
    check_not_adj_ctrl();
    check_not_free(qubit);

    *qubit.allocated_ = false;

    block_map[current_block].add_instruction(
        dirty? 
        quantum_code::intruction_t{
            quantum_code::op_code_t::free_dirty,
            qubit.index()
        } : 
        quantum_code::intruction_t{
            quantum_code::op_code_t::free,
            qubit.index()
        }
    );

    num_qubit -= 1;
    max_num_qubit = num_qubit > max_num_qubit? num_qubit : max_num_qubit;

    if (dirty) add_feature(features_t::qubit_free_dirty);
    else       add_feature(features_t::qubit_free);
}



void process_t::gate(gate_t gate_, qubit_t qubit, double param) {
    check_not_free(qubit);
    check_process_id(qubit);

    auto feature_select = [&](auto f0, auto f1, auto f2) {
        if (ctrl_stack.size() > 1) {
            return f2;
        } else if (ctrl_stack.size() == 1 and ctrl_stack.back().size() == 1) {
            return f1;
        } else {
            return f0;
        }
    };
    
    if (not block_map[current_block].adj() and 
        (gate_ == gate_t::phase or 
         gate_ == gate_t::rotation_x or 
         gate_ == gate_t::rotation_y or 
         gate_ == gate_t::rotation_z)) {
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::set_param,
            param
        });
    }

    switch (gate_) {
    case gate_t::pauli_x:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_pauli_x,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_pauli_x,
            features_t::gate_ctrl_pauli_x,
            features_t::gate_mult_ctrl_pauli_x
        ));
        break;
    case gate_t::pauli_y:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_pauli_y,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_pauli_y,
            features_t::gate_ctrl_pauli_y,
            features_t::gate_mult_ctrl_pauli_y
        ));
        break;
    case gate_t::pauli_z:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_pauli_z,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_pauli_z,
            features_t::gate_ctrl_pauli_z,
            features_t::gate_mult_ctrl_pauli_z
        ));
        break;
    case gate_t::hadamard:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_hadamard,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_hadamard,
            features_t::gate_ctrl_hadamard,
            features_t::gate_mult_ctrl_hadamard
        ));
        break;
    case gate_t::phase:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_phase,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_phase,
            features_t::gate_ctrl_phase,
            features_t::gate_mult_ctrl_phase
        ));
        break;
    case gate_t::rotation_x:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_rotation_x,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_rotation_x,
            features_t::gate_ctrl_rotation_x,
            features_t::gate_mult_ctrl_rotation_x
        ));
        break;
    case gate_t::rotation_y:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_pauli_y,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_rotation_y,
            features_t::gate_ctrl_rotation_y,
            features_t::gate_mult_ctrl_rotation_y
        ));
        break;
    case gate_t::rotation_z:
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::gate_pauli_z,
            qubit.index()
        });
        add_feature(feature_select(
            features_t::gate_rotation_z,
            features_t::gate_ctrl_rotation_z,
            features_t::gate_mult_ctrl_rotation_z
        ));
        break;
    }

    if (block_map[current_block].adj() and 
        (gate_ == gate_t::phase or 
         gate_ == gate_t::rotation_x or 
         gate_ == gate_t::rotation_y or 
         gate_ == gate_t::rotation_z)) {
        block_map[current_block].add_instruction({
            quantum_code::op_code_t::set_param,
            -param
        });
    }
}

future_t process_t::measure(qubit_list_t qubit_list) {
    check_not_adj_ctrl();

    for (auto qubit : qubit_list) {
        check_not_free(qubit);
        check_process_id(qubit);
        *qubit.measured_ = true;
        block_map[current_block].add_instruction(
            quantum_code::intruction_t{
                quantum_code::op_code_t::push_qubit,
                qubit.index()
            }
        );
    }

    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::measure,
            future_count
        }
    );

    future_map[future_count] = future_t{future_count, this, process_id(), alive};
    return future_map[future_count++];
}

future_t process_t::new_int(int_t value) {
    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::push_int,
            future_count
        }
    );

    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::int_const,
            value
        }
    );

    future_map[future_count] = future_t{future_count, this, process_id(), alive};
    return future_map[future_count++];
}


void process_t::plugin(std::string name, qubit_list_t qubit_list, std::string args) {
    for (auto qubit : qubit_list) {
        check_not_free(qubit);
        check_process_id(qubit);
        check_not_in_ctrl(qubit);
        block_map[current_block].add_instruction(
            quantum_code::intruction_t{
                quantum_code::op_code_t::push_qubit,
                qubit.index()
            }
        );
    }

    block_map[current_block].add_instruction({
        quantum_code::op_code_t::get_plugin_args,
        block_map[current_block].add_string(args)
    });

    block_map[current_block].add_instruction({
        quantum_code::op_code_t::plugin,
        block_map[current_block].add_string(block_map[current_block].adj()? "!"+name : name)
    });

    add_feature(features_t::plugin);
}

void process_t::ctrl_push(qubit_list_t qubit_list) {
    for (auto qubit : qubit_list) {
        check_not_free(qubit);
        check_process_id(qubit);
        check_not_in_ctrl(qubit);
    }
    ctrl_stack.push_back(qubit_list);

    if (block_map[current_block].adj()) {
        ctrl_holder.push(qubit_list);
        block_map[current_block].add_instruction(
            quantum_code::intruction_t{
                quantum_code::op_code_t::pop_ctrl,
                static_cast<index_t>(qubit_list.size())
            }
        );
    } else for (auto qubit : qubit_list) {
        block_map[current_block].add_instruction(
            quantum_code::intruction_t{
                quantum_code::op_code_t::push_ctrl,
                qubit.index()
            }
        );
    }

}

void process_t::ctrl_pop() {
    if (ctrl_stack.empty()) {
        throw not_in_ctrl{};
    }

    if (block_map[current_block].adj()) {
        for (auto qubit : ctrl_holder.top()) {
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::push_ctrl,
                    qubit.index()
                }
            );
        }
        ctrl_holder.pop();
    } else {
        block_map[current_block].add_instruction(
            quantum_code::intruction_t{
                quantum_code::op_code_t::pop_ctrl,
                static_cast<index_t>(ctrl_stack.back().size())
            }
        );
    }
    ctrl_stack.pop_back();
}

void process_t::adj_begin() {
    block_map[current_block].adj_begin();
}

void process_t::adj_end() {
    if (not ctrl_holder.empty()) throw open_ctrl{};
    block_map[current_block].adj_end();
}

label_t process_t::get_label() {
    return label_t{label_count++, process_id()};
}

void process_t::open_block(label_t label) {
    check_process_id(label);
    current_block = label.index();
}

void process_t::jump(label_t label) {
    check_process_id(label);
    block_map[current_block].add_instruction({
        quantum_code::op_code_t::jump,
        label.index()    
    });
    block_map[current_block].end();
}

void process_t::breach(future_t test, label_t then, label_t otherwise) {
    check_process_id(test);
    check_process_id(then);
    check_process_id(otherwise);

    block_map[current_block].add_instruction({
        quantum_code::op_code_t::set_then,
        then.index()    
    });
    block_map[current_block].add_instruction({
        quantum_code::op_code_t::set_else,
        otherwise.index()    
    });
    block_map[current_block].add_instruction({
        quantum_code::op_code_t::breach,
        test.index()    
    });
    block_map[current_block].end();
}

dump_t process_t::dump(qubit_list_t qubit_list) {
    for (auto qubit : qubit_list) {
        check_not_free(qubit);
        check_process_id(qubit);
        block_map[current_block].add_instruction(
            quantum_code::intruction_t{
                quantum_code::op_code_t::push_qubit,
                qubit.index()
            }
        );
    } 
    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::dump,
            dump_count
        }
    ); 

    add_feature(features_t::quantum_dump);

    dump_map[dump_count] = dump_t{dump_count, this, process_id(), alive};
    return dump_map[dump_count++];
}

future_t process_t::int_op(int_op_t int_op, future_t left, future_t right) {
    check_not_adj_ctrl();   
    check_process_id(left);
    check_process_id(right);

    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::push_int,
            left.index()
        }
    );

    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::push_int,
            right.index()
        }
    );

    add_feature(features_t::int_operations);
    switch (int_op) {
        case int_op_t::eq:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_eq,
                    future_count
                }
            );
            break;
        case int_op_t::neq:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_neq,
                    future_count
                }
            );
            break;
        case int_op_t::gt:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_gt,
                    future_count
                }
            );
            break;
        case int_op_t::geq:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_geq,
                    future_count
                }
            );
            break;
        case int_op_t::lt:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_lt,
                    future_count
                }
            );
            break;
        case int_op_t::leq:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_leq,
                    future_count
                }
            );
            break;
        case int_op_t::add:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_add,
                    future_count
                }
            );
            break;
        case int_op_t::sub:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_sub,
                    future_count
                }
            );
            break;
        case int_op_t::mul:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_mul,
                    future_count
                }
            );
            break;
        case int_op_t::div:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_div,
                    future_count
                }
            );
            break;
        case int_op_t::sll:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_sll,
                    future_count
                }
            );
            break;
        case int_op_t::srl:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_srl,
                    future_count
                }
            );
            break;
        case int_op_t::and_:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_and,
                    future_count
                }
            );
            break;
        case int_op_t::or_:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_or,
                    future_count
                }
            );
            break;
        case int_op_t::xor_:
            block_map[current_block].add_instruction(
                quantum_code::intruction_t{
                    quantum_code::op_code_t::int_xor,
                    future_count
                }
            );
            break;
        case int_op_t::set:
            throw std::runtime_error{"invalid param \"int_op=int_op_t::set\""};
            break;
    }

    future_map[future_count] = future_t{future_count, this, process_id(), alive};
    return future_map[future_count++];
}

void process_t::int_set(future_t dest, future_t src) {
    check_not_adj_ctrl();
    check_process_id(dest);
    check_process_id(src);    

    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::push_int,
            src.index()
        }
    );

    block_map[current_block].add_instruction(
        quantum_code::intruction_t{
            quantum_code::op_code_t::int_set,
            dest.index()
        }
    );
    add_feature(features_t::int_set);
}


double process_t::exec_time() const {
    return exec_time_;
}

ket::index_t process_t::process_id() const {
    return pid;
}

void process_t::check_not_adj_ctrl() {
    auto in_ctrl = not ctrl_stack.empty();
    auto in_adj = not block_map[current_block].not_adj();
    if (in_ctrl or in_adj) {
        throw not_allowed_in_ctrl_adj{};
    }
}

void process_t::check_not_free(qubit_t qubit) {
    if (not qubit.allocated()) {
        throw free_qubit{};
    }
}

void process_t::check_process_id(qubit_t qubit) {
    if (qubit.process_id() != process_id()) {
        throw process_id_not_match{};
    }
}

void process_t::check_process_id(future_t future) {
    if (future.process_id() != process_id()) {
        throw process_id_not_match{};
    }
}

void process_t::check_process_id(label_t label) {
    if (label.process_id() != process_id()) {
        throw process_id_not_match{};
    }
}

void process_t::check_not_in_ctrl(qubit_t qubit) {
    for (auto &ctrl_list : ctrl_stack) for (auto &ctrl_qubit : ctrl_list) if (qubit.index() == ctrl_qubit.index()) {
        throw qubit_in_ctrl{};
    }
}

void process_t::add_feature(features_t new_feature) {
    features = static_cast<features_t>(features|new_feature);
}

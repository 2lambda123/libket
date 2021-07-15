/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/ket"
#include <algorithm>
#include <iomanip>
#include <gmp.h>

using namespace ket;

dump::dump(const quant& q) :
    nbits{q.len()},
    process_on_top{process_on_top_stack.top()}
{
    if (not *(q.process_on_top))
        throw std::runtime_error("process out of scope");

    auto [id, states, available] = process_stack.top()->dump(q.qubits);
    
    this->id = id;
    this->states = states;
    this->available = available;
}

std::vector<std::vector<unsigned long>> dump::get_states() {
    if (not *available) get();

    std::vector<std::vector<unsigned long>> states_key;
    for (auto &i : *states) states_key.push_back(i.first);

    return states_key;
}

std::vector<std::complex<double>> dump::amplitude(std::vector<unsigned long> idx) {
    if (not *available) get();

    return states->at(idx);
}

void dump::get() {
    if (*available) return;

    if (not *process_on_top)
        throw std::runtime_error("process out of scope");

    exec_quantum();
}

double dump::probability(std::vector<unsigned long> idx) {
    double p = 0;
    for (auto i : amplitude(idx)) p += std::abs(i*i);
    return p;
}

inline double sqrt_apx(double value) {
    return 1.0/(value*value);
}

std::string dump::show(std::string format) {
    if (not *available) get();

    std::vector<std::pair<bool, unsigned>> forms;

    auto reg_sum = 0u;
    std::stringstream format_buffer(format);
    std::string reg_str;
    while (std::getline(format_buffer, reg_str, ':')) {
        if (reg_str[0] != 'b' and reg_str[0] != 'i') 
            throw std::invalid_argument("The format string must be 'b|i<number>[...]'.");

        bool binary = reg_str[0] == 'b';
        auto reg_tmp = std::stoul(reg_str.substr(1));
        forms.push_back(std::make_pair(binary, reg_tmp));    
        reg_sum += reg_tmp;
    }
    
    if (reg_sum > nbits)
        throw std::invalid_argument("Format string out of bounds.");

    if (nbits-reg_sum != 0) forms.push_back(std::make_pair(true, nbits-reg_sum));

    std::stringstream out;
    for (auto i : get_states()) {
        auto begin = 0u;
        for (auto [binary, nbits_reg] : forms) {
            out << '|';

            std::stringstream value;
            for (auto j = begin; j < begin+nbits_reg; j++) {
                auto index = nbits-j-1;
                auto base_index = index/64;
                auto bit_index = index%64;
                value << (i[base_index] & 1ul << bit_index? '1' : '0');
            }
            begin += nbits_reg; 


            if (binary) {
                out << value.str();
            } else {
                mpz_t bigint;
                mpz_init(bigint);

                mpz_set_str(bigint, value.str().c_str(), 2);
                auto value_str = mpz_get_str(nullptr, 10, bigint);

                out << value_str;
                
                free(value_str);
                mpz_clear(bigint);
            }
            out << "⟩";
        }
        out << "\t\t(" << probability(i)*100.0 << "%)" << std::endl;
        
        for (auto j : amplitude(i)) {
            std::stringstream cx;
            if (std::abs(j.real()) > 1e-10) {
                cx << j.real();
                if (std::abs(j.imag()) > 1e-10) 
                    cx << (j.imag() < 0? " " : " +") << j.imag() << "i";
            } else if (std::abs(j.imag()) > 1e-10) {
                cx << j.imag() << "i";
            }
            out << std::left << std::setw(25) << std::setfill(' ') << cx.str() << "≅ ";   
            auto real = std::abs(j.real()) < 1e-10? 0 : sqrt_apx(j.real());
            auto imag = std::abs(j.imag()) < 1e-10? 0 : sqrt_apx(j.imag());

            if (std::abs(real-imag) < 1e-5) {
                out << (j.real() < 0.0? "(-" : "(") << '1' << (j.imag() < 0.0? '-' : '+') << "i)/√" << real << std::endl;
            } else {
                if (real > 1e-10) {
                    out << (j.real() < 0.0?  '-' : ' ') << "1/√" << real;
                } 
                if (imag > 1e-10) {
                    out << (j.imag() < 0.0? '-' : '+') << "i/√" << imag;
                }
                out << std::endl;
            } 
        } 

        out << std::endl;
    }
    
    return out.str();
}

bool dump::operator==(dump& other) {
    auto this_states = get_states();
    auto other_states = other.get_states();
    
    if (this_states.size() != other_states.size()) return false;

    for (auto i = 0u; i < this_states.size(); i++) if (this_states[i] != other_states[i]) return false;

    //for (auto i : this_states) {
    //    auto this_amp = amplitude(i);
    //    auto other_amp = other.amplitude(i);
    //
    //    if (this_amp.size() != other_amp.size()) return false;
    //
    //    for (auto j = 0u; j < this_amp.size(); j ++) if (std::abs(this_amp[j]-other_amp[j]) > 1e-10) return false;
    //} 

    return true;
}

bool dump::operator!=(dump& other) {
    return not (*this == other);
}
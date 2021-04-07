/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

std::vector<std::complex<double>> dump::amplitude(std::vector<unsigned long>& idx) {
    if (not *available) get();

    return states->at(idx);
}

void dump::get() {
    if (*available) return;

    if (not *process_on_top)
        throw std::runtime_error("process out of scope");

    exec_quantum();
}

double dump::probability(std::vector<unsigned long>& idx) {
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
            cx << j.real() << (j.imag() < 0? " " : " +") << j.imag() << "i";
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
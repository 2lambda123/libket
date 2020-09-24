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
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <fstream>
#include <algorithm>

using namespace ket;

template <class T>
inline boost::array<char, sizeof(T)> to_char(T value) {
    boost::array<char, sizeof(T)> buffer;
    std::memcpy(&buffer[0], &value, sizeof(T));
    return buffer;
}

template <class T>
inline T from_char(boost::array<char, sizeof(T)> buffer) {
    T value;
    std::memcpy(&value, &buffer[0], sizeof(T));
    return value;
}

inline auto conect(const std::string& addr, int port) {
    boost::asio::io_service ios;
    boost::asio::ip::tcp::endpoint server(boost::asio::ip::address::from_string(addr), port);
    boost::asio::ip::tcp::socket socket(ios);

    socket.connect(server);
    
    return socket;
}

inline auto buffer_str(const std::string& input) {
    std::vector<char> buffer;
    for (char i : input) buffer.push_back(i);
    return buffer;
}

void process::exec() {
    
    if (output_kqasm) {
        std::ofstream out{kqasm_path, std::ofstream::app};
        out << kqasm.str() 
            << "=========================" 
            << std::endl;
        out.close();
    }

    if (execute_kqasm) {
        
        auto kqasm_buffer = buffer_str(kqasm.str());
        auto kqasm_size = to_char<uint32_t>(kqasm_buffer.size());
        
        char ack[1];

        auto socket = conect(kbw_addr, kbw_port);

        socket.send(boost::asio::buffer(kqasm_size));
        socket.receive(boost::asio::buffer(ack));

        socket.send(boost::asio::buffer(kqasm_buffer));
        socket.receive(boost::asio::buffer(ack));
        
        auto get_command = to_char<char>(1);
        for (auto &i : measure_map) {
            socket.send(boost::asio::buffer(get_command));
            socket.receive(boost::asio::buffer(ack));
         
            auto get_idx = to_char<uint64_t>(i.first);
            socket.send(boost::asio::buffer(get_idx));
            socket.receive(boost::asio::buffer(get_idx));

            *(i.second.first) = from_char<uint64_t>(get_idx);
            
            *(i.second.second) = true;
        }
        
        auto dump_command = to_char<char>(2);
        boost::array<char, 8> buffer;
        for (auto &i : dump_map) {
            socket.send(boost::asio::buffer(dump_command));
            socket.receive(boost::asio::buffer(ack));

            auto idx = to_char<uint64_t>(i.first);
            socket.send(boost::asio::buffer(idx));
            socket.receive(boost::asio::buffer(buffer));
            auto size = from_char<uint64_t>(buffer);

            ack[0] = 0;
            socket.send(boost::asio::buffer(ack));
            
            for (auto j = 0u; j < size; j++) {
                
                socket.receive(boost::asio::buffer(buffer));
                auto basis = from_char<uint64_t>(buffer); 
                
                socket.receive(boost::asio::buffer(buffer));
                auto amp_size = from_char<uint64_t>(buffer); 

                ack[0] = 0;
                socket.send(boost::asio::buffer(ack));

                for (auto k = 0u; k < amp_size; k++) {

                    socket.receive(boost::asio::buffer(buffer));
                    auto real = from_char<double>(buffer); 

                    socket.receive(boost::asio::buffer(buffer));
                    auto imag = from_char<double>(buffer); 
                    
                    ack[0] = 0;
                    socket.send(boost::asio::buffer(ack));

                    (*i.second.first)[basis].push_back(std::complex<double>{real, imag});
                } 

                std::sort((*i.second.first)[basis].begin(), (*i.second.first)[basis].end(), [](std::complex<double> a, std::complex<double> b) {
                    if (a.real() == b.real()) return a.imag() < b.imag();
                    else return a.real() < b.real();
                });
            }
            
            *i.second.second = true;
        } 

        auto exit_command = to_char<int>(0);
        socket.send(boost::asio::buffer(exit_command));
        socket.receive(boost::asio::buffer(ack));

        socket.close();
    } else for (auto &i : measure_map) {
        *(i.second.first) =  0;
        *(i.second.second) = true;
    }
}

void ket::exec_quantum() {
    process_stack.top()->exec();

    process_stack.pop();

    *process_on_top_stack.top() = false;
    process_on_top_stack.pop();

    process_stack.push(std::make_shared<process>());
    process_on_top_stack.push(std::make_shared<bool>(true));
}

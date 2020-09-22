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

using namespace ket;

template <class T, int N>
inline boost::array<char, N> to_char(T value) {
    boost::array<char, N> buffer;
    std::memcpy(&buffer[0], &value, N);
    return buffer;
}

template <class T, int N>
inline T from_char(boost::array<char, N> buffer) {
    T value;
    std::memcpy(&value, &buffer[0], N);
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
        auto kqasm_size = to_char<int, 4>(kqasm_buffer.size());
        
        char ack[1];

        auto socket = conect(kbw_addr, kbw_port);

        socket.send(boost::asio::buffer(kqasm_size));
        socket.receive(boost::asio::buffer(ack));

        socket.send(boost::asio::buffer(kqasm_buffer));
        socket.receive(boost::asio::buffer(ack));
        
        auto get_command = to_char<int, 4>(1);
        for (auto &i : measure_map) {
            socket.send(boost::asio::buffer(get_command));
            socket.receive(boost::asio::buffer(ack));
         
            auto get_idx = to_char<size_t, 8>(i.first);
            socket.send(boost::asio::buffer(get_idx));
            socket.receive(boost::asio::buffer(get_idx));

            *(i.second.first) = from_char<size_t, 8>(get_idx);
            
            *(i.second.second) = true;
        }

        auto exit_command = to_char<int, 4>(0);
        socket.send(boost::asio::buffer(exit_command));
        socket.receive(boost::asio::buffer(ack));

        socket.close();
    } else for (auto &i : measure_map) {
        *(i.second.first) =  0;
        *(i.second.second) = true;
    }
}

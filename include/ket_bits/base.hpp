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

#pragma once
#include <vector>
#include <memory>
#include <sstream>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <functional>
#include <queue>
#include <stack>

namespace ket::base {
    class i64;

    enum class result {
        ZERO = 0,
        ONE  = 1,
        NONE = 3,
    };

    class gate {
    public:
        enum TAG {X,  Y,  Z,
                  H,  S,  T,
                  U1, U2, U3,
                  PLUGIN,
                  DUMP, 
                  MEASURE,  
                  ALLOC, 
                  FREE,
                  JUMP, BR,
                  LABEL,
                  WAIT};

        gate(TAG tag, 
             size_t qubit_idx, 
             bool adj_dirty = false,
             const std::shared_ptr<gate>& back = nullptr,
             const std::vector<double>& args = {}, 
             const std::vector<size_t>& ctrl_idx = {},
             const std::vector<std::shared_ptr<gate>>& ctrl_back = {});
        
        gate(TAG tag,
             const std::vector<size_t>& ctrl_idx,
             const std::vector<std::shared_ptr<gate>>& ctrl_back,
             const std::string& label1 = "",
             const std::string& label2 = "",
             const std::shared_ptr<i64>& bri64 = nullptr,
             const std::vector<std::shared_ptr<i64>>& back_i64 = {});

        void eval(std::stringstream& circuit);

    private:
        TAG tag;
        size_t qubit_idx;
        std::shared_ptr<gate> back;
        bool adj_dirty;
        std::vector<double> args;

        std::vector<size_t> ctrl_idx;
        std::vector<std::shared_ptr<gate>> ctrl_back;

        std::string label, label_false;

        std::shared_ptr<i64> bri64;

        std::vector<std::shared_ptr<i64>> back_i64;

        bool visit;
    };

    class qubit {
    public:
        qubit(size_t qubit_idx);

        size_t idx();

        void add_gate(const std::shared_ptr<gate>& new_gate);
        std::shared_ptr<gate> last_gate();

    private:

        size_t qubit_idx;

        std::shared_ptr<gate> tail_gate;
    };

    class bit {
    public:
        bit(size_t bit_idx,
            std::shared_ptr<gate> measurement_gate);
        
        size_t idx() const;

        void eval(std::stringstream& circuit);

    private:

        size_t bit_idx;
        std::shared_ptr<gate> measurement_gate;

        bool visit;
    };

    class i64 {
    public:
        i64(const std::vector<std::shared_ptr<bit>>& bits, 
            size_t i64_idx,
            bool se = false);
        i64(const std::string& op, 
            const std::vector<std::shared_ptr<i64>>& args, 
            size_t i64_idx,
            bool infix = true);
        i64(std::int64_t value, size_t i64_idx);
        
        i64(const std::vector<std::shared_ptr<i64>>& args);

        bool has_value();
        std::int64_t get_value();
        void set_value(std::int64_t value);

        void set_label(const std::shared_ptr<base::gate>& label);

        void eval(std::stringstream& circuit);

        size_t idx() const;

    private:
        enum TAG { BIT, TMP, VALUE, ASS, PC } tag;

        std::vector<std::shared_ptr<bit>> bits;
        bool se;
        size_t i64_idx;
        
        std::string op;
        std::vector<std::shared_ptr<i64>> args;
        bool infix;

        std::int64_t value;

        std::shared_ptr<base::gate> label;

        bool visit;
    };

    class _process {
    public:
        _process();    
        std::shared_ptr<qubit> alloc(bool dirty = false);

        void add_gate(gate::TAG gate_tag, 
                      const std::shared_ptr<qubit>& qbit, 
                      const std::vector<double>& args = {});

        void add_plugin_gate(const std::string &gate_name, 
                             const std::vector<std::shared_ptr<qubit>>& qbit,
                             const std::string& args = "");

        void wait();

        std::shared_ptr<bit> measure(const std::shared_ptr<qubit>& qbit);

        void free(const std::shared_ptr<qubit>& qbit, bool dirty = false);

        std::shared_ptr<i64> new_i64(const std::vector<std::shared_ptr<bit>>& bits);

        std::shared_ptr<i64> i64_op(const std::string& op, 
                                    const std::vector<std::shared_ptr<i64>>& args, 
                                    bool infix=true);
                                    
        std::shared_ptr<i64> const_i64(std::int64_t value);

        void set_i64(const std::shared_ptr<i64>& target, const std::shared_ptr<i64>& value);

        void adj_begin();
        
        void adj_end();

        void ctrl_begin(const std::vector<std::shared_ptr<qubit>>& ctrl);
        void ctrl_end();

        void begin_block(const std::string& next_label);

        std::shared_ptr<ket::base::gate> end_block(const std::string& label_goto1,
                                                   const std::string& label_goto2 = "",
                                                   const std::shared_ptr<i64>& bri64 = nullptr);
         
        void eval();

    private:
        size_t qubit_count;
        size_t bit_count;
        size_t i64_count;

        std::string label;

        boost::unordered_map<size_t, std::shared_ptr<qubit>> qubit_map;
        boost::unordered_map<size_t, std::shared_ptr<i64>> measurement_map;

        std::stack<std::stack<std::function<void()>>> adj_call;

        std::vector<std::vector<size_t>> ctrl_qubit;

        std::queue<std::function<void()>> block_call;
        boost::unordered_set<size_t> block_free;
        std::vector<std::shared_ptr<i64>> block_i64;
        
    };

}

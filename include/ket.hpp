#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <memory>
#include <sstream>
#include <variant>
#include <functional>
#include <iostream>

namespace ket::base {
    class Handler {
    public:
      enum Result {
          ZERO = 0,
          ONE = 1,
          NONE = 3,
      };

      class Qubits {
        public:
          Qubits(const std::vector<Qubits>& bits); 
          Qubits operator[](size_t index) const;
          size_t size() const;
          const std::vector<size_t>::const_iterator begin() const;
          const std::vector<size_t>::const_iterator end() const;

        private:
          Qubits(const std::vector<size_t>& qubits);

          std::vector<size_t> qubits;

          friend class Handler;
      };

      class Bits {
        public:
          Bits(const std::vector<Bits>& list);
          Bits operator[](size_t index) const;

          size_t size() const;
          Handler::Result get(size_t index);      

        private:
          Bits(Handler& handler, const std::vector<size_t>& bits, const std::vector<std::shared_ptr<Result>>& measurement);

          Handler &handler;
          std::vector<size_t> bits;
          std::vector<std::shared_ptr<Result>> measurement;

          friend class Handler;
      };
    
      class Qubit_alloc {
        public:
          Qubit_alloc(size_t qubit_index, bool dirty=false);

        private:
          boost::unordered_set<size_t> qubit_index;
          std::stringstream circuit;
          boost::unordered_map<size_t, std::shared_ptr<Result>> measurement_return;

          friend class Handler;
      };

      Handler(const std::string& out_path, const std::string& kbw_path, const std::string& kcq_path, size_t seed, bool no_execute, bool no_optimise);
      ~Handler();
      
      Qubits alloc(size_t size, bool dirty=false);
      void add_gate(const std::string& gate, const Qubits& qubits, const std::vector<double>& args={});
      void __add_gate(const std::string& gate, const Qubits& qubits, const std::vector<double>& args, bool adj, const std::vector<Qubits>& _qctrl, const std::vector<Bits>& _cctrl);
      void add_oracle(const std::string& gate, const Qubits& qubits);
      Bits measure(const Qubits& qubits);
      void free(const Qubits& qubits);
      void free_dirty(const Qubits& qubits);
      void ctrl_begin();
      void add_ctrl(const Qubits& qubits);
      void add_ctrl(const Bits& qubits);
      void ctrl_end();
      void adj_begin();
      void adj_end();
      void __run(const Bits& bits);

    private:

      std::stringstream& merge(const Qubits& qubits);

      bool out_to_file;
      std::string out_path;
      std::ofstream out_file;
      std::string kbw_path;
      std::string kqc_path;
      size_t quantum_counter;
      size_t classical_counter;
      boost::unordered_map<size_t, std::shared_ptr<Qubit_alloc>> allocations;
      boost::unordered_map<size_t, size_t> measure_map;
      size_t seed;
      bool no_execute;
      bool no_optimise;
      std::vector<Qubits> qctrl;
      std::vector<Bits> cctrl;
      std::vector<bool> qctrl_b;
      std::vector<bool> cctrl_b;
      std::vector<std::vector<std::function<void()>>> adj_call;
    };
}

extern "C" ket::base::Handler* ket_handle;
#define ket_init ket::base::Handler* ket_handle

namespace ket {

    void begin(int argc, char* argv[]);
    void end();

    class Bit;
    class Qubit {
     public:
        Qubit(size_t size=1);
        Qubit operator() (size_t index) const;
        Qubit operator() (size_t begin, size_t end) const;
        size_t size() const;

     private:
        Qubit(const base::Handler::Qubits& qubits);

        base::Handler::Qubits qubits;

        friend void x(const Qubit& q);
        friend void y(const Qubit& q);
        friend void z(const Qubit& q);
        friend void h(const Qubit& q);
        friend void s(const Qubit& q);
        friend void sd(const Qubit& q);
        friend void t(const Qubit& q);
        friend void td(const Qubit& q);
        friend void cnot(const Qubit& ctrl, const Qubit& target);
        friend void u1(double lambda, const Qubit& q);
        friend void u2(double phi, double lambda, const Qubit& q);
        friend void u3(double theta, double phi, double lambda, const Qubit& q);
        friend Qubit operator+(const Qubit& a, const Qubit& b);
        friend Bit measure(const Qubit& q);
        friend Qubit dirty(size_t size);
        friend void free(const Qubit& q);
        friend void freedirty(const Qubit& q);
        friend void __apply_oracle(const std::string& gate, const Qubit& q);
        friend class Qubit_or_Bit;
    };

    class Bit {
     public:
        Bit operator() (size_t index) const;
        Bit operator() (size_t begin, size_t end) const;

        int operator[] (size_t index);

        size_t size() const;

     private:
        Bit(const base::Handler::Bits& bits);
        
        base::Handler::Bits bits;

        friend Bit measure(const Qubit& q);
        friend Bit operator+(const Bit& a, const Bit& b);
        friend class Qubit_or_Bit;
    };

    class Qubit_or_Bit {
     public:
        Qubit_or_Bit(const Qubit& qubit);
        Qubit_or_Bit(const Bit& bit);

        bool quantum() const;
        base::Handler::Qubits get_qubit() const;
        base::Handler::Bits get_bit() const;

     private:
        std::variant<Qubit, Bit> bit;
        bool _quantum;
    };

    template <class T, class F, class... Args> 
    T ctrl(const std::vector<Qubit_or_Bit>& c, F func, Args... args) {
        ket_handle->ctrl_begin();
        for (const auto &i: c) {
            if (i.quantum()) {
                ket_handle->add_ctrl(i.get_qubit());
            } else {
                ket_handle->add_ctrl(i.get_bit());
            }
        }
        T result = func(args...);
        ket_handle->ctrl_end();
        return result;
    }    

    template <class F, class... Args> 
    void ctrl(const std::vector<Qubit_or_Bit>& c, F func, Args... args) {
        ket_handle->ctrl_begin();
        for (const auto &i: c) {
            if (i.quantum()) {
                ket_handle->add_ctrl(i.get_qubit());
            } else {
                ket_handle->add_ctrl(i.get_bit());
            }
        }
        func(args...);
        ket_handle->ctrl_end();
    }   

    template <class F, class... Args>
    void adj(F func, Args...  args) {
        ket_handle->adj_begin();
        func(args...);
        ket_handle->adj_end();
    } 

    void x(const Qubit& q);
    void y(const Qubit& q);
    void z(const Qubit& q);
    void h(const Qubit& q);
    void s(const Qubit& q);
    void sd(const Qubit& q);
    void t(const Qubit& q);
    void td(const Qubit& q);
    void cnot(const Qubit& ctrl, const Qubit& target);
    void u1(double lambda, const Qubit& q);
    void u2(double phi, double lambda, const Qubit& q);
    void u3(double theta, double phi, double lambda, const Qubit& q);
    Qubit operator+(const Qubit& a, const Qubit& b);
    Bit operator+(const Bit& a, const Bit& b);
    std::ostream& operator<<(std::ostream& os, Bit bit); 
    Bit measure(const Qubit& q);
    Qubit dirty(size_t size);
    void free(const Qubit& q);
    void freedirty(const Qubit& q);
    void __apply_oracle(const std::string& gate, const Qubit& q);
}

namespace ket::oracle {
    using gate_map = boost::unordered_map<size_t, boost::unordered_set<std::pair<std::complex<double>, size_t>>>;
    class Gate {
     public:
        Gate(gate_map& gate, size_t size); 
        boost::unordered_set<std::pair<std::complex<double>, size_t>>& operator[](size_t index);
        size_t size() const;

        void operator()(const ket::Qubit& q);

     private:
        gate_map gate;
        size_t _size;
    };

    Gate lambda(std::function<size_t(size_t)> func, size_t size, size_t begin=0, size_t end=0);
}
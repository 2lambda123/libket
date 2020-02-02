#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <memory>
#include <sstream>
#include <variant>
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
          Qubit_alloc(size_t qubit_index);

        private:
          boost::unordered_set<size_t> qubit_index;
          std::stringstream circuit;
          boost::unordered_map<size_t, std::shared_ptr<Result>> measurement_return;

          friend class Handler;
      };

      Handler(const std::string& out_path, const std::string& kbw_path, size_t seed, bool no_execute);
      ~Handler();
      
      Qubits alloc(size_t size);
      void add_gate(std::string gate, const Qubits& qubits);
      Bits measure(const Qubits& qubits);
      void ctrl_begin();
      void add_ctrl(const Qubits& qubits);
      void add_ctrl(const Bits& qubits);
      void ctrl_end();
      void __run(const Bits& bits);

    private:

      std::stringstream& merge(const Qubits& qubits);

      bool out_to_file;
      std::string out_path;
      std::ofstream out_file;
      std::string kbw_path;
      size_t quantum_counter;
      size_t classical_counter;
      boost::unordered_map<size_t, std::shared_ptr<Qubit_alloc>> allocations;
      boost::unordered_map<size_t, size_t> measure_map;
      size_t seed;
      bool no_execute;
      std::vector<Qubits> qctrl;
      std::vector<Bits> cctrl;
      std::vector<bool> qctrl_b;
      std::vector<bool> cctrl_b;
    };
}

namespace ket {

    extern std::unique_ptr<base::Handler> handle;
    #define ket_init std::unique_ptr<ket::base::Handler> ket::handle

    void init(int argc, char* argv[]);

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
        friend Qubit operator+(const Qubit& a, const Qubit& b);
        friend Bit measure(const Qubit& q);
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

    void ctrl_begin(const std::vector<Qubit_or_Bit>& c);

    void ctrl_end();

    template <class T, class F, class... Args> 
    T ctrl(const std::vector<Qubit_or_Bit>& c, F func, Args... args) {
        ctrl_begin(c);
        T result = func(args...);
        ctrl_end();
        return result;
    }    

    template <class F, class... Args> 
    void ctrl(const std::vector<Qubit_or_Bit>& c, F func, Args... args) {
        ctrl_begin(c);
        func(args...);
        ctrl_end();
    }   

    template <class T>
    T to(Bit bit) {
      T ret{};
      auto size = bit.size();
      for (size_t i = 0; i < size; i++) {
        ret |= bit[i] << (size-i-1);
      }
      return ret;
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
    Qubit operator+(const Qubit& a, const Qubit& b);
    Bit operator+(const Bit& a, const Bit& b);
    Bit measure(const Qubit& q);

}
#Licensed under the Apache License, Version 2.0;
#Copyright 2022 Evandro Chagas Ribeiro da Rosa
from ctypes import *

def load_libketc():
    from os import environ
    from os.path import dirname

    if "LIBKETC_PATH" in environ:
        libketc_path = environ["LIBKETC_PATH"]
    else:
        libketc_path = dirname(__file__)+"/libketc.so"

    return cdll.LoadLibrary(libketc_path)

def set_kbw_path():
    from os import environ
    from os.path import dirname

    if "KET_QUANTUM_EXECUTOR" not in environ:
        environ["KET_QUANTUM_EXECUTOR"] = dirname(__file__)+"/kbw.so"

class libket_error(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

KET_SUCCESS    = 0
KET_ERROR      = 1
KET_PAULI_X    = 2 
KET_PAULI_Y    = 3   
KET_PAULI_Z    = 4
KET_ROTATION_X = 5
KET_ROTATION_Y = 6
KET_ROTATION_Z = 7
KET_HADAMARD   = 8
KET_PHASE      = 9
KET_INT_EQ     = 10
KET_INT_NEQ    = 11
KET_INT_GT     = 12
KET_INT_GEQ    = 13
KET_INT_LT     = 14
KET_INT_LEQ    = 15
KET_INT_ADD    = 16
KET_INT_SUB    = 17
KET_INT_MUL    = 18
KET_INT_DIV    = 19
KET_INT_SLL    = 20
KET_INT_SRL    = 21
KET_INT_AND    = 22
KET_INT_OR     = 23
KET_INT_XOR    = 24
KET_INT_FF     = 25
KET_INT_FI     = 26
KET_INT_IF     = 27

set_kbw_path()

libketc = load_libketc()
ket_error_message = libketc.ket_error_message
ket_error_message.argtypes = []
ket_error_message.restype = c_char_p

def ket_error_warpper(error : c_int):
    if error == KET_ERROR:
        raise libket_error(str(ket_error_message().decode()))

class qubit_t:
    ket_qubit_new = libketc.ket_qubit_new
    ket_qubit_new.argtypes = [POINTER(c_void_p)]

    ket_qubit_delete = libketc.ket_qubit_delete
    ket_qubit_delete.argtypes = [c_void_p]

    ket_qubit_index = libketc.ket_qubit_index
    ket_qubit_index.argtypes = [c_void_p, POINTER(c_uint)]

    ket_qubit_measured = libketc.ket_qubit_measured
    ket_qubit_measured.argtypes = [c_void_p, POINTER(c_bool)]

    ket_qubit_allocated = libketc.ket_qubit_allocated
    ket_qubit_allocated.argtypes = [c_void_p, POINTER(c_bool)]

    ket_qubit_process_id = libketc.ket_qubit_process_id
    ket_qubit_process_id.argtypes = [c_void_p, POINTER(c_uint)]

    def __init__(self):
        self._as_parameter_ = c_void_p()
        ket_error_warpper(
            self.ket_qubit_new(byref(self._as_parameter_))
        )

    def __del__(self):
        ket_error_warpper(
            self.ket_qubit_delete(self)
        )

    @property
    def index(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_qubit_index(self, c_value)
        )
        return c_value.value

    @property
    def measured(self):
        c_value = c_bool()
        ket_error_warpper(
            self.ket_qubit_measured(self, c_value)
        )
        return c_value.value

    @property
    def allocated(self):
        c_value = c_bool()
        ket_error_warpper(
            self.ket_qubit_allocated(self, c_value)
        )
        return c_value.value

    @property
    def process_id(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_qubit_process_id(self, c_value)
        )
        return c_value.value

class dump_t:
    ket_dump_new = libketc.ket_dump_new
    ket_dump_new.argtypes = [POINTER(c_void_p)]

    ket_dump_delete = libketc.ket_dump_delete
    ket_dump_delete.argtypes = [c_void_p]

    ket_dump_states = libketc.ket_dump_states
    ket_dump_states.argtypes = [c_void_p, POINTER(c_void_p), POINTER(c_size_t)]

    ket_dump_state_at = libketc.ket_dump_state_at
    ket_dump_state_at.argtypes = [c_void_p, POINTER(POINTER(c_ulong)), POINTER(c_size_t), c_ulong]

    ket_dump_amplitides = libketc.ket_dump_amplitides
    ket_dump_amplitides.argtypes = [c_void_p, POINTER(c_void_p), POINTER(c_size_t)]
    
    ket_dump_amp_at = libketc.ket_dump_amp_at
    ket_dump_amp_at.argtypes = [c_void_p, POINTER(c_void_p), POINTER(c_size_t), c_ulong]

    ket_dump_amp = libketc.ket_dump_amp
    ket_dump_amp.argtypes = [c_void_p, POINTER(c_double), POINTER(c_double), c_ulong]

    ket_dump_available = libketc.ket_dump_available
    ket_dump_available.argtypes = [c_void_p, POINTER(c_bool)]

    ket_dump_index = libketc.ket_dump_index
    ket_dump_index.argtypes = [c_void_p, POINTER(c_uint)]

    ket_dump_process_id = libketc.ket_dump_process_id
    ket_dump_process_id.argtypes = [c_void_p, POINTER(c_uint)]

    def __init__(self):
        self._as_parameter_ = c_void_p()
        ket_error_warpper(
            self.ket_dump_new(byref(self._as_parameter_))
        )

    def __del__(self):
        ket_error_warpper(
            self.ket_dump_delete(self)
        )
    
    def states(self):
        c_states = c_void_p()
        c_size   = c_size_t()
        ket_error_warpper(
            self.ket_dump_states(self, c_states, c_size)
        )
        for i in range(c_size.value):
            c_state = POINTER(c_ulong)()
            c_state_size = c_size_t()
            ket_error_warpper(
                self.ket_dump_state_at(c_states, c_state, c_state_size, i)
            )
            yield int(''.join(f'{c_state[j]:064b}' for j in range(c_state_size.value)), 2)

    def amplitides(self):
        c_amplitides = c_void_p()
        c_size       = c_size_t()
        ket_error_warpper(
            self.ket_dump_amplitides(self, c_amplitides, c_size)
        )
        for i in range(c_size.value):
            c_amplitide = c_void_p()
            c_amplitide_size = c_size_t()
            ket_error_warpper(
                self.ket_dump_amp_at(c_amplitides, c_amplitide, c_amplitide_size, i)
            )
            amplitide = []
            for j in range(c_amplitide_size.value):
                c_real = c_double()
                c_imag = c_double()
                ket_error_warpper(
                    self.ket_dump_amp(c_amplitide, c_real, c_imag, j)
                )
                amplitide.append(c_real.value+c_imag.value*1j)
            
            yield amplitide if len(amplitide) != 1 else amplitide[0]

    @property
    def available(self):
        c_value = c_bool()()
        ket_error_warpper(
            self.ket_dump_available(self, c_value)
        )
        return c_value.value

    @property
    def index(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_dump_index(self, c_value)
        )
        return c_value.value

    @property
    def process_id(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_dump_process_id(self, c_value)
        )
        return c_value.value

class future_t:
    ket_future_new = libketc.ket_future_new
    ket_future_new.argtypes = [POINTER(c_void_p)]

    ket_future_delete = libketc.ket_future_delete
    ket_future_delete.argtypes = [c_void_p]

    ket_future_value = libketc.ket_future_value
    ket_future_value.argtypes = [c_void_p, POINTER(c_long)]

    ket_future_set = libketc.ket_future_set
    ket_future_set = [c_void_p, c_void_p]

    ket_future_available = libketc.ket_future_available
    ket_future_available.argtypes = [c_void_p, POINTER(c_bool)]

    ket_future_index = libketc.ket_future_index
    ket_future_index.argtypes = [c_void_p, POINTER(c_uint)]

    ket_future_process_id = libketc.ket_future_process_id
    ket_future_process_id.argtypes = [c_void_p, POINTER(c_uint)]

    ket_future_op = libketc.ket_future_op

    def __init__(self):
        self._as_parameter_ = c_void_p()
        ket_error_warpper(
            self.ket_future_new(byref(self._as_parameter_))
        )

    def __del__(self):
        ket_error_warpper(
            self.ket_future_delete(self)
        )

    def __getattr__(self, name):
        if name != "value":
            raise AttributeError(name)
        value = c_long()
        ket_error_warpper(
            self.ket_future_value(self, value)
        )
        return value.value
    
    def __setattr__(self, name, value):
        if name == "value":
            ket_error_warpper(
                self.ket_future_set(self, value)
            )
        else:
            super().__setattr__(name, value)

    @property
    def available(self):
        c_value = c_bool()()
        ket_error_warpper(
            self.ket_future_available(self, c_value)
        )
        return c_value.value

    @property
    def index(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_future_index(self, c_value)
        )
        return c_value.value

    @property
    def process_id(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_future_process_id(self, c_value)
        )
        return c_value.value

    def __add__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_ADD, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_ADD, KET_INT_FI, self, int(other))
            )
        return result

    def __sub__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_ADD, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_ADD, KET_INT_FI, self, int(other))
            )
        return result

    def __mul__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_MUL, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_MUL, KET_INT_FI, self, int(other))
            )
        return result

    def __truediv__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_DIV, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_DIV, KET_INT_FI, self, int(other))
            )
        return result

    def __floordiv__(self, other):
        return self.__truediv__(other)

    def __lshift__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_SLL, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_SLL, KET_INT_FI, self, int(other))
            )
        return result

    def __rshift__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_SRL, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_SRL, KET_INT_FI, self, int(other))
            )
        return result

    def __and__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_AND, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_AND, KET_INT_FI, self, int(other))
            )
        return result

    def __xor__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_XOR, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_XOR, KET_INT_FI, self, int(other))
            )
        return result

    def __or__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_OR, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_OR, KET_INT_FI, self, int(other))
            )
        return result

    def __radd__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_ADD, KET_INT_IF, int(other), self)
        )
        return result

    def __rsub__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_SUB, KET_INT_IF, int(other), self)
        )
        return result

    def __rmul__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_MUL, KET_INT_IF, int(other), self)
        )
        return result

    def __rtruediv__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_DIV, KET_INT_IF, int(other), self)
        )
        return result

    def __rfloordiv__(self, other):
        return self.__rtruediv__(other)

    def __rlshift__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_SLL, KET_INT_IF, int(other), self)
        )
        return result

    def __rrshift__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_SRL, KET_INT_IF, int(other), self)
        )
        return result

    def __rand__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_AND, KET_INT_IF, int(other), self)
        )
        return result

    def __rxor__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_XOR, KET_INT_IF, int(other), self)
        )
        return result

    def __ror__(self, other):
        self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_long, c_void_p]
        result = future_t()
        ket_error_warpper(
            self.ket_future_op(result, KET_INT_OR, KET_INT_IF, int(other), self)
        )
        return result

    def __lt__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_LT, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_LT, KET_INT_FF, self, int(other))
            )
        return result

    def __le__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_LEQ, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_LEQ, KET_INT_FF, self, int(other))
            )
        return result

    def __eq__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_EQ, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_EQ, KET_INT_FF, self, int(other))
            )
        return result

    def __ne__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_NEQ, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_NEQ, KET_INT_FF, self, int(other))
            )
        return result

    def __gt__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_GT, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_GT, KET_INT_FF, self, int(other))
            )
        return result

    def __ge__(self, other):
        result = future_t()
        if isinstance(other, future_t):
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_void_p]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_GEQ, KET_INT_FF, self, other)
            )
        else:
            self.ket_future_op.argtypes = [c_void_p, c_int, c_int, c_void_p, c_long]
            ket_error_warpper(
                self.ket_future_op(result, KET_INT_GEQ, KET_INT_FF, self, int(other))
            )
        return result


class label_t:
    ket_label_new = libketc.ket_label_new
    ket_label_new.argtypes = [POINTER(c_void_p)]

    ket_label_delete = libketc.ket_label_delete
    ket_label_delete.argtypes = [c_void_p]

    ket_label_index = libketc.ket_label_index
    ket_label_index.argtypes = [c_void_p, POINTER(c_uint)]

    ket_label_process_id = libketc.ket_label_process_id
    ket_label_process_id.argtypes = [c_void_p, POINTER(c_uint)]

    def __init__(self):
        self._as_parameter_ = c_void_p()
        ket_error_warpper(
            self.ket_label_new(byref(self._as_parameter_))
        )

    def __del__(self):
        ket_error_warpper(
            self.ket_label_delete(self)
        )

    @property
    def index(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_label_index(self, c_value)
        )
        return c_value.value

    @property
    def process_id(self):
        c_value = c_uint()
        ket_error_warpper(
            self.ket_label_process_id(self, c_value)
        )
        return c_value.value

class process_t:
    ket_process_new = libketc.ket_process_new
    ket_process_new.argtypes = [POINTER(c_void_p), c_uint]

    ket_process_delete = libketc.ket_process_delete
    ket_process_delete.argtypes = [c_void_p]

    ket_process_alloc = libketc.ket_process_alloc
    ket_process_alloc.argtypes = [c_void_p, c_void_p, c_bool]

    ket_process_gate = libketc.ket_process_gate
    ket_process_gate.argtypes = [c_void_p, c_int, c_void_p, c_double]

    ket_process_measure = libketc.ket_process_measure

    ket_process_ctrl_push = libketc.ket_process_ctrl_push

    ket_process_ctrl_pop = libketc.ket_process_ctrl_pop
    ket_process_ctrl_pop.argtypes = [c_void_p]

    ket_process_adj_begin = libketc.ket_process_adj_begin
    ket_process_adj_begin.argtypes = [c_void_p]

    ket_process_adj_end = libketc.ket_process_adj_end
    ket_process_adj_end.argtypes = [c_void_p]

    ket_process_get_label = libketc.ket_process_get_label
    ket_process_get_label.argtypes = [c_void_p, c_void_p]

    ket_process_open_block = libketc.ket_process_open_block
    ket_process_open_block.argtypes = [c_void_p, c_void_p]

    ket_process_jump = libketc.ket_process_jump
    ket_process_jump.argtypes = [c_void_p, c_void_p]

    ket_process_breach = libketc.ket_process_breach
    ket_process_breach.argtypes = [c_void_p, c_void_p, c_void_p]

    ket_process_dump = libketc.ket_process_dump

    ket_process_run = libketc.ket_process_run
    ket_process_run.argtypes = [c_void_p]

    ket_process_exec_time = libketc.ket_process_exec_time
    ket_process_exec_time.argtypes = [c_void_p, POINTER(c_double)]

    ket_process_id = libketc.ket_process_id
    ket_process_id.argtypes = [c_void_p, POINTER(c_uint)]

    def __init__(self, process_id : int):
        self._as_parameter_ = c_void_p()
        ket_error_warpper(
            self.ket_process_new(byref(self._as_parameter_), process_id)
        )

    def __del__(self):
        ket_error_warpper(
            self.ket_process_delete(self)
        )
    
    def alloc(self, dirty = False):
        qubit = qubit_t()
        ket_error_warpper(
            self.ket_process_alloc(self, qubit, dirty)
        )
        return qubit    

    def gate(self, gate : int, qubit : qubit_t, param : float = 0):
        ket_error_warpper(
            self.ket_process_gate(self, gate, qubit, param)
        )
    
    def measure(self, *qubits):
        self.ket_process_measure.argtypes = [c_void_p, c_void_p, c_int] + [c_void_p for _ in range(len(qubits))]
        future = future_t()
        ket_error_warpper(
            self.ket_process_measure(self, future, len(qubits), *qubits)
        )
        return future

    def ctrl_push(self, *qubits):
        self.ket_process_ctrl_push.argtypes = [c_void_p, c_int] + [c_void_p for _ in range(len(qubits))]
        ket_error_warpper(
            self.ket_process_ctrl_push(self, len(qubits), *qubits)
        )

    def ctrl_pop(self):
        ket_error_warpper(
            self.ket_process_ctrl_pop(self)
        )

    def adj_begin(self):
        ket_error_warpper(
            self.ket_process_adj_begin(self)
        )

    def adj_end(self):
        ket_error_warpper(
            self.ket_process_adj_end(self)
        )

    def get_label(self):
        label = label_t()
        ket_error_warpper(
            self.ket_process_get_label(self, label)
        )
        return label

    def open_block(self, label : label_t):
        ket_error_warpper(
            self.ket_process_open_block(self, label)
        )

    def jump(self, label : label_t):
        ket_error_warpper(
            self.ket_process_jump(self, label)
        )

    def breach(self, test : future_t, then : label_t, otherwise : label_t):
        ket_error_warpper(
            self.ket_process_breach(self, test, then, otherwise)
        )

    def run(self):
        ket_error_warpper(
            self.ket_process_run(self)
        )

    @property
    def exec_time(self):
        value = c_double()
        ket_error_warpper(
            self.ket_process_exec_time(self, value)
        )
        return value.value

    @property
    def id(self):
        value = c_uint()
        ket_error_warpper(
            self.ket_process_id(self, value)
        )
        return value.value    
    
    def dump(self, *qubits):
        self.ket_process_dump.argtypes = [c_void_p, c_void_p, c_int] + [c_void_p for _ in range(len(qubits))]
        dump_ = dump_t()
        ket_error_warpper(
            self.ket_process_dump(self, dump_, len(qubits), *qubits)
        )
        return dump_

    
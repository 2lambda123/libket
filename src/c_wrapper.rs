use crate::*;

const KET_SUCCESS: i32 = 0;
const KET_ERROR: i32 = 1;

static mut ERROR_MESSAGE: String = String::new();

#[no_mangle]
pub extern "C" fn ket_error_message(size: &mut usize) -> *const u8 {
    *size = unsafe { ERROR_MESSAGE.len() };
    unsafe { ERROR_MESSAGE.as_ptr() }
}

#[no_mangle]
pub extern "C" fn ket_process_new(pid: u32) -> *mut Process {
    Box::into_raw(Box::new(Process::new(pid)))
}

#[no_mangle]
pub extern "C" fn ket_process_delete(process: *mut Process) {
    unsafe { Box::from_raw(process) };
}

#[no_mangle]
pub extern "C" fn ket_process_alloc(process: &mut Process, dirty: bool) -> *mut Qubit {
    match process.alloc(dirty) {
        Ok(qubit) => Box::into_raw(Box::new(qubit)),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            0 as *mut Qubit
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_free(process: &mut Process, qubit: &mut Qubit, dirty: bool) -> i32 {
    match process.free(qubit, dirty) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_apply_gate(
    process: &mut Process,
    gate: u32,
    param: f64,
    target: &Qubit,
) -> i32 {
    let gate = match gate {
        0 => QuantumGate::PauliX,
        1 => QuantumGate::PauliY,
        2 => QuantumGate::PauliZ,
        3 => QuantumGate::Hadamard,
        4 => QuantumGate::Phase(param),
        5 => QuantumGate::RX(param),
        6 => QuantumGate::RY(param),
        7 => QuantumGate::RZ(param),
        _ => {
            unsafe { ERROR_MESSAGE = String::from("Undefined quantum gate.") };
            return KET_ERROR;
        }
    };
    match process.apply_gate(gate, target) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_apply_plugin(
    process: &mut Process,
    name: *const u8,
    name_size: usize,
    target: *const &Qubit,
    target_size: usize,
    args: *const u8,
    args_size: usize,
) -> i32 {
    let name = unsafe { std::slice::from_raw_parts(name, name_size) };
    let name = match std::str::from_utf8(name) {
        Ok(name) => name,
        Err(msg) => {
            unsafe {
                ERROR_MESSAGE = format!("{}", msg);
            }
            return KET_ERROR;
        }
    };

    let target = unsafe { std::slice::from_raw_parts(target, target_size) };

    let args = unsafe { std::slice::from_raw_parts(args, args_size) };
    let args = match std::str::from_utf8(args) {
        Ok(args) => args,
        Err(msg) => {
            unsafe {
                ERROR_MESSAGE = format!("{}", msg);
            }
            return KET_ERROR;
        }
    };

    match process.apply_plugin(name, target, args) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe {
                ERROR_MESSAGE = msg;
            }
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_measure(
    process: &mut Process,
    qubits: *mut &mut Qubit,
    num_qubits: usize,
) -> *mut Future {
    let qubits = unsafe { std::slice::from_raw_parts_mut(qubits, num_qubits) };
    match process.measure(qubits) {
        Ok(future) => Box::into_raw(Box::new(future)),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            0 as *mut Future
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_ctrl_push(
    process: &mut Process,
    qubits: *const &Qubit,
    num_qubits: usize,
) -> i32 {
    let qubits = unsafe { std::slice::from_raw_parts(qubits, num_qubits) };
    match process.ctrl_push(qubits) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_ctrl_pop(process: &mut Process) -> i32 {
    match process.ctrl_pop() {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_adj_begin(process: &mut Process) -> i32 {
    match process.adj_begin() {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_adj_end(process: &mut Process) -> i32 {
    match process.adj_end() {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_get_label(process: &mut Process) -> *mut Label {
    match process.get_label() {
        Ok(label) => Box::into_raw(Box::new(label)),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            0 as *mut Label
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_open_block(process: &mut Process, label: &Label) -> i32 {
    match process.open_block(label) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_jump(process: &mut Process, label: &Label) -> i32 {
    match process.jump(label) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_branch(
    process: &mut Process,
    test: &Future,
    then: &Label,
    otherwise: &Label,
) -> i32 {
    match process.branch(test, then, otherwise) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_dump(
    process: &mut Process,
    qubits: *const &Qubit,
    num_qubits: usize,
) -> *mut Dump {
    let qubits = unsafe { std::slice::from_raw_parts(qubits, num_qubits) };
    match process.dump(&qubits) {
        Ok(dump) => Box::into_raw(Box::new(dump)),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            0 as *mut Dump
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_add_int_op(
    process: &mut Process,
    op: u32,
    lhs: &Future,
    rhs: &Future,
) -> *mut Future {
    let op = match op {
        0 => ClassicalOp::Eq,
        1 => ClassicalOp::Neq,
        2 => ClassicalOp::Gt,
        3 => ClassicalOp::Geq,
        4 => ClassicalOp::Lt,
        5 => ClassicalOp::Leq,
        6 => ClassicalOp::Add,
        7 => ClassicalOp::Sub,
        8 => ClassicalOp::Mul,
        9 => ClassicalOp::Div,
        10 => ClassicalOp::Sll,
        11 => ClassicalOp::Srl,
        12 => ClassicalOp::And,
        13 => ClassicalOp::Or,
        14 => ClassicalOp::Xor,
        _ => {
            unsafe { ERROR_MESSAGE = String::from("Undefined classical operation.") };
            return 0 as *mut Future;
        }
    };
    match process.add_int_op(op, lhs, rhs) {
        Ok(result) => Box::into_raw(Box::new(result)),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            0 as *mut Future
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_int_set(
    process: &mut Process,
    result: &Future,
    value: &Future,
) -> i32 {
    match process.int_set(result, value) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_int_new(process: &mut Process, value: i64) -> *mut Future {
    match process.int_new(value) {
        Ok(future) => Box::into_raw(Box::new(future)),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = String::from(msg) };
            0 as *mut Future
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_exec_time(process: &Process, available: &mut bool) -> f64 {
    match process.exec_time() {
        Some(time) => {
            *available = true;
            time
        }
        None => {
            *available = false;
            0.0
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_set_timeout(process: &mut Process, timeout: u32) {
    process.set_timeout(timeout);
}

#[no_mangle]
pub extern "C" fn ket_process_get_quantum_code_as_json(
    process: &mut Process,
    size: &mut usize,
) -> *const u8 {
    match process.get_quantum_code_as_json() {
        Ok(json) => {
            *size = json.len();
            json.as_ptr()
        }
        Err(msg) => {
            *size = 0;
            unsafe { ERROR_MESSAGE = String::from(msg) };
            0 as *const u8
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_get_quantum_code_as_bin(
    process: &mut Process,
    size: &mut usize,
) -> *const u8 {
    match process.get_quantum_code_as_bin() {
        Ok(bin) => {
            *size = bin.len();
            bin.as_ptr()
        }
        Err(msg) => {
            *size = 0;
            unsafe { ERROR_MESSAGE = String::from(msg) };
            0 as *const u8
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_get_metrics_as_json(
    process: &mut Process,
    size: &mut usize,
) -> *const u8 {
    let metrics = process.get_metrics_as_json();
    *size = metrics.len();
    metrics.as_bytes().as_ptr()
}

#[no_mangle]
pub extern "C" fn ket_process_get_metrics_as_bin(
    process: &mut Process,
    size: &mut usize,
) -> *const u8 {
    let metrics = process.get_metrics_as_bin();
    *size = metrics.len();
    metrics.as_ptr()
}

#[no_mangle]
pub extern "C" fn ket_process_set_quantum_result_from_json(
    process: &mut Process,
    result: *const u8,
    size: usize,
) -> i32 {
    let result = unsafe { std::slice::from_raw_parts(result, size) };
    let result = match std::str::from_utf8(result) {
        Ok(result) => result,
        Err(msg) => {
            unsafe {
                ERROR_MESSAGE = format!("Fail to read result from json: {}", msg);
            }
            return KET_ERROR;
        }
    };

    match process.set_quantum_result_from_json(result) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe {
                ERROR_MESSAGE = msg;
            }
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_process_set_quantum_result_from_bin(
    process: &mut Process,
    result: *const u8,
    size: usize,
) -> i32 {
    let result = unsafe { std::slice::from_raw_parts(result, size) };

    match process.set_quantum_result_from_bin(result) {
        Ok(_) => KET_SUCCESS,
        Err(msg) => {
            unsafe {
                ERROR_MESSAGE = msg;
            }
            KET_ERROR
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_qubit_delete(qubit: *mut Qubit) {
    unsafe { Box::from_raw(qubit) };
}

#[no_mangle]
pub extern "C" fn ket_qubit_index(qubit: &Qubit) -> u32 {
    qubit.index()
}

#[no_mangle]
pub extern "C" fn ket_qubit_pid(qubit: &Qubit) -> u32 {
    qubit.pid()
}

#[no_mangle]
pub extern "C" fn ket_qubit_allocated(qubit: &Qubit) -> bool {
    qubit.allocated()
}

#[no_mangle]
pub extern "C" fn ket_qubit_measured(qubit: &Qubit) -> bool {
    qubit.measured()
}

#[no_mangle]
pub extern "C" fn ket_dump_delete(dump: *mut Dump) {
    unsafe { Box::from_raw(dump) };
}

#[no_mangle]
pub extern "C" fn ket_dump_states(dump: &Dump, size: &mut usize) -> *const Vec<u64> {
    match dump.value().as_ref() {
        Some(value) => {
            *size = value.basis_states().len();
            value.basis_states().as_ptr()
        }
        None => {
            *size = 0;
            0 as *const Vec<u64>
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn ket_dump_get_state(
    states: *const Vec<u64>,
    index: usize,
    size: &mut usize,
) -> *const u64 {
    let state = states.add(index);
    *size = (*state).len();
    (*state).as_ptr()
}

#[no_mangle]
pub extern "C" fn ket_dump_amplitudes_real(dump: &Dump, size: &mut usize) -> *const f64 {
    match dump.value().as_ref() {
        Some(value) => {
            *size = value.amplitudes_real().len();
            value.amplitudes_real().as_ptr()
        }
        None => {
            *size = 0;
            0 as *const f64
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_dump_amplitudes_img(dump: &Dump, size: &mut usize) -> *const f64 {
    match dump.value().as_ref() {
        Some(value) => {
            *size = value.amplitudes_img().len();
            value.amplitudes_img().as_ptr()
        }
        None => {
            *size = 0;
            0 as *const f64
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_dump_available(dump: &Dump) -> bool {
    dump.value().is_some()
}

#[no_mangle]
pub extern "C" fn ket_future_delete(future: *mut Future) {
    unsafe { Box::from_raw(future) };
}

#[no_mangle]
pub extern "C" fn ket_future_value(future: &Future, available: &mut bool) -> i64 {
    match future.value().as_ref() {
        Some(value) => {
            *available = true;
            *value
        }
        None => {
            *available = false;
            -1
        }
    }
}

#[no_mangle]
pub extern "C" fn ket_future_index(future: &Future) -> u32 {
    future.index()
}

#[no_mangle]
pub extern "C" fn ket_future_pid(future: &Future) -> u32 {
    future.pid()
}

#[no_mangle]
pub extern "C" fn ket_label_delete(label: *mut Label) {
    unsafe { Box::from_raw(label) };
}

#[no_mangle]
pub extern "C" fn ket_label_index(label: &Label) -> u32 {
    label.index()
}

#[no_mangle]
pub extern "C" fn ket_label_pid(label: &Label) -> u32 {
    label.pid()
}

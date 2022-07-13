use crate::{
    error::KetError,
    object::{Dump, Future, Label, Pid, Qubit},
};

#[no_mangle]
pub extern "C" fn ket_qubit_delete(qubit: *mut Qubit) -> i32 {
    unsafe { Box::from_raw(qubit) };
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_qubit_index(qubit: &Qubit, index: &mut usize) -> i32 {
    *index = qubit.index();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_qubit_pid(qubit: &Qubit, pid: &mut usize) -> i32 {
    *pid = qubit.pid();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_qubit_allocated(qubit: &Qubit, allocated: &mut bool) -> i32 {
    *allocated = qubit.allocated();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_qubit_measured(qubit: &Qubit, measured: &mut bool) -> i32 {
    *measured = qubit.measured();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_dump_delete(dump: *mut Dump) -> i32 {
    unsafe { Box::from_raw(dump) };
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_dump_states(
    dump: &Dump,
    states: *mut *const Vec<u64>,
    size: &mut usize,
) -> i32 {
    match dump.value().as_ref() {
        Some(value) => {
            *size = value.basis_states().len();
            unsafe {
                *states = value.basis_states().as_ptr();
            }
            KetError::Success.error_code()
        }
        None => KetError::DataNotAvailable.error_code(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn ket_dump_get_state(
    states: *const Vec<u64>,
    index: usize,
    state: *mut *const u64,
    size: &mut usize,
) -> i32 {
    let states = states.add(index);
    *size = (*states).len();
    *state = (*states).as_ptr();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_dump_amplitudes_real(
    dump: &Dump,
    amp: *mut *const f64,
    size: &mut usize,
) -> i32 {
    match dump.value().as_ref() {
        Some(value) => {
            *size = value.amplitudes_real().len();
            unsafe {
                *amp = value.amplitudes_real().as_ptr();
            }
            KetError::Success.error_code()
        }
        None => KetError::DataNotAvailable.error_code(),
    }
}

#[no_mangle]
pub extern "C" fn ket_dump_amplitudes_img(
    dump: &Dump,
    amp: *mut *const f64,
    size: &mut usize,
) -> i32 {
    match dump.value().as_ref() {
        Some(value) => {
            *size = value.amplitudes_img().len();
            unsafe {
                *amp = value.amplitudes_img().as_ptr();
            }
            KetError::Success.error_code()
        }
        None => KetError::DataNotAvailable.error_code(),
    }
}

#[no_mangle]
pub extern "C" fn ket_dump_available(dump: &Dump, available: &mut bool) -> i32 {
    *available = dump.value().is_some();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_future_delete(future: *mut Future) -> i32 {
    unsafe { Box::from_raw(future) };
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_future_value(future: &Future, value: &mut i64) -> i32 {
    match future.value().as_ref() {
        Some(data) => {
            *value = *data;
            KetError::Success.error_code()
        }
        None => KetError::DataNotAvailable.error_code(),
    }
}

#[no_mangle]
pub extern "C" fn ket_future_index(future: &Future, index: &mut usize) -> i32 {
    *index = future.index();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_future_pid(future: &Future, pid: &mut usize) -> i32 {
    *pid = future.pid();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_future_available(future: &Future, available: &mut bool) -> i32 {
    *available = future.value().is_some();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_label_delete(label: *mut Label) -> i32 {
    unsafe { Box::from_raw(label) };
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_label_index(label: &Label, index: &mut usize) -> i32 {
    *index = label.index();
    KetError::Success.error_code()
}

#[no_mangle]
pub extern "C" fn ket_label_pid(label: &Label, pid: &mut usize) -> i32 {
    *pid = label.pid();
    KetError::Success.error_code()
}

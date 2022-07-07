use std::{
    cell::{Ref, RefCell},
    rc::Rc,
};

use serde::{Deserialize, Serialize};

use crate::error::KetError;

pub trait Pid {
    fn pid(&self) -> usize;
}

#[derive(Debug)]
pub struct Qubit {
    index: usize,
    pid: usize,
    allocated: bool,
    measured: bool,
}

impl Qubit {
    pub fn new(index: usize, pid: usize) -> Qubit {
        Qubit {
            index,
            pid,
            allocated: true,
            measured: false,
        }
    }

    pub fn index(&self) -> usize {
        self.index
    }

    pub fn allocated(&self) -> bool {
        self.allocated
    }

    pub fn set_deallocated(&mut self) {
        self.allocated = false;
    }

    pub fn measured(&self) -> bool {
        self.measured
    }

    pub fn set_measured(&mut self) {
        self.measured = false;
    }

    pub fn assert_allocated(&self) -> Result<(), KetError> {
        if !self.allocated {
            Err(KetError::DeallocatedQubit)
        } else {
            Ok(())
        }
    }
}

impl Pid for Qubit {
    fn pid(&self) -> usize {
        self.pid
    }
}

#[derive(Debug)]
pub struct Future {
    index: usize,
    pid: usize,
    value: Rc<RefCell<Option<i64>>>,
}

impl Future {
    pub fn new(index: usize, pid: usize, value: Rc<RefCell<Option<i64>>>) -> Future {
        Future { index, pid, value }
    }

    pub fn value(&self) -> Ref<Option<i64>> {
        self.value.borrow()
    }

    pub fn index(&self) -> usize {
        self.index
    }
}

impl Pid for Future {
    fn pid(&self) -> usize {
        self.pid
    }
}

#[derive(Debug, Serialize, Deserialize)]
pub struct DumpData {
    pub basis_states: Vec<Vec<u64>>,
    pub amplitudes_real: Vec<f64>,
    pub amplitudes_img: Vec<f64>,
}

impl DumpData {
    pub fn basis_states(&self) -> &[Vec<u64>] {
        &self.basis_states
    }

    pub fn amplitudes_real(&self) -> &[f64] {
        &self.amplitudes_real
    }

    pub fn amplitudes_img(&self) -> &[f64] {
        &self.amplitudes_img
    }
}

#[derive(Debug)]
pub struct Dump {
    value: Rc<RefCell<Option<DumpData>>>,
}

impl Dump {
    pub fn new(value: Rc<RefCell<Option<DumpData>>>) -> Dump {
        Dump { value }
    }

    pub fn value(&self) -> Ref<Option<DumpData>> {
        self.value.borrow()
    }
}

pub struct Label {
    index: usize,
    pid: usize,
}

impl Label {
    pub fn new(index: usize, pid: usize) -> Label {
        Label { index, pid }
    }

    pub fn index(&self) -> usize {
        self.index
    }
}

impl Pid for Label {
    fn pid(&self) -> usize {
        self.pid
    }
}

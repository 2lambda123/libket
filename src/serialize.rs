use num_derive::{FromPrimitive, ToPrimitive};

#[derive(FromPrimitive, ToPrimitive)]
pub enum DataType {
    JSON,
    BIN,
}

pub enum SerializedData {
    JSON(String),
    BIN(Vec<u8>),
}

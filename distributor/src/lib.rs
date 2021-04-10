pub mod content_range;
pub use content_range::ContentRange;

mod db;
use db::Db;

pub const DEFAULT_PORT: &str = "8080";

pub mod server;

pub type Error = Box<dyn std::error::Error + Send + Sync>;

pub type Result<T> = std::result::Result<T, Error>;

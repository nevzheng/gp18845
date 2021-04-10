use distributor::{server, DEFAULT_PORT};
use structopt::StructOpt;

#[tokio::main]
pub async fn main() -> distributor::Result<()> {
    // enable logging
    // see https://docs.rs/tracing for more info
    // Tracing should be used for complex async situations
    // BUT dbg! is simpler, quicker, more useful
    tracing_subscriber::fmt::try_init()?;

    // Determine Port from Command Line Arguments
    let cli = Cli::from_args();
    let port: &str = cli.port.as_deref().unwrap_or(DEFAULT_PORT);

    server::run("127.0.0.1", port).await
}

// Uses structopt cargo package
#[derive(StructOpt, Debug)]
#[structopt(
    name = "distributor",
    version = env!("CARGO_PKG_VERSION"),
    author = env!("CARGO_PKG_AUTHORS"),
    about = "A Distributor aggregating and distributing content feeds"
)]
struct Cli {
    #[structopt(name = "port", long = "--port")]
    port: Option<String>,
}

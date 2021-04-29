extern crate rocket;
extern crate rocket_contrib;

use rocket_contrib::serve::StaticFiles;

fn main() {
    let root = std::path::PathBuf::from(&std::env::var("CONTENT_ROOT").unwrap());
    rocket::ignite()
        .mount("/", StaticFiles::from(root))
        .launch();
}

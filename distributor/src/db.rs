use bytes::Bytes;
use std::collections::HashMap;
use std::sync::{Arc, RwLock};

use crate::ContentRange;

#[derive(Clone)]
pub(crate) struct Db {
    shared: Arc<Shared>,
}

struct Shared {
    state: RwLock<State>,
}

struct State {
    // Use a HashMap for Now
    entries: HashMap<ContentRange, Entry>,
}

#[derive(Debug)]
struct Entry {
    data: Arc<Bytes>,
}

impl Db {
    pub(crate) fn new() -> Self {
        let shared = Arc::new(Shared {
            state: RwLock::new(State {
                entries: HashMap::new(),
            }),
        });
        Db { shared }
    }

    pub(crate) fn set(&self, key: ContentRange, value: Bytes) {
        let mut state = self.shared.state.write().unwrap();
        state.entries.insert(
            key,
            Entry {
                data: Arc::new(value),
            },
        );
    }

    pub(crate) fn get(&self, key: &ContentRange) -> Option<Arc<Bytes>> {
        let state = self.shared.state.read().unwrap();
        state.entries.get(key).map(|entry| entry.data.clone())
    }
}

use hyper::header::HeaderValue;
use lazy_static::lazy_static;
use regex::Regex;
use std::str::FromStr;

#[derive(Hash, Debug, Eq, PartialEq)]
enum ContentRangeUnit {
    Bytes,
}

#[derive(Hash, Debug, Eq, PartialEq)]
pub struct ContentRange {
    unit: ContentRangeUnit,
    start: usize,
    end: usize,
    size: usize,
}

impl ContentRange {
    pub fn length(&self) -> usize {
        self.end - self.start
    }

    pub fn start(&self) -> usize {
        self.size
    }

    pub fn value(&self) -> String {
        format!("{} {}-{}/{}", "bytes", self.start, self.end, self.size)
    }
}

impl FromStr for ContentRangeUnit {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s == "bytes" {
            Ok(ContentRangeUnit::Bytes)
        } else {
            Err(())
        }
    }
}

impl From<&HeaderValue> for ContentRange {
    fn from(header_value: &HeaderValue) -> Self {
        lazy_static! {
            static ref MATCH_CONTENT_RANGE: Regex =
                Regex::new(r"(?P<units>\w+) (?P<start>\d+)-(?P<end>\d+)/(?P<size>\d+)")
                    .expect("Could not generate regex");
        }

        let captures = MATCH_CONTENT_RANGE
            .captures(header_value.to_str().unwrap())
            .expect("Regex Failed to Parse Content-Range");

        let unit = captures
            .name("units")
            .expect("Could Not Match Content-Range Unit")
            .as_str()
            .parse()
            .expect("Could Not Parse Content-Range Unit");

        let start = captures
            .name("start")
            .expect("Could Not match Content-Range Start")
            .as_str()
            .parse()
            .expect("Could Not Parse Content-Range Start");

        let end = captures
            .name("end")
            .expect("Could Not match Content-Range End")
            .as_str()
            .parse()
            .expect("Could Not Parse Content-Range End");

        let size = captures
            .name("size")
            .expect("Could Not match Content-Range Size")
            .as_str()
            .parse()
            .expect("Could Not Parse Content-Range Size");

        Self {
            unit,
            start,
            end,
            size,
        }
    }
}

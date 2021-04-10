use std::net::SocketAddr;

use hyper::http::header::CONTENT_RANGE;
use hyper::service::{make_service_fn, service_fn};
use hyper::{Body, Method, Request, Response, Server, StatusCode};

use crate::{ContentRange, Db};

async fn request_router(req: Request<Body>, db_handle: Db) -> crate::Result<Response<Body>> {
    let (parts, body) = req.into_parts();
    let content_range_value = parts.headers.get(CONTENT_RANGE).expect("Invalid Post");
    let content_range = ContentRange::from(content_range_value);
    match (parts.method, parts.uri.path()) {
        (Method::POST, "/origin") => handle_post(db_handle, content_range, body).await,
        (Method::GET, "/origin") => handle_get(db_handle, content_range),
        _ => Ok(Response::new(Body::from("Can only operate on /origin"))),
    }
}

async fn handle_post(
    db_handle: Db,
    content_range: ContentRange,
    body: Body,
) -> crate::Result<Response<Body>> {
    let bytes = hyper::body::to_bytes(body)
        .await
        .expect("Error Reading ByteStream");

    db_handle.set(content_range, bytes);
    Ok(Response::builder()
        .status(StatusCode::OK)
        .body(Body::empty())
        .unwrap())
}

fn handle_get(db_handle: Db, content_range: ContentRange) -> crate::Result<Response<Body>> {
    // This is inefficient as it requires cloning
    // Can fix by implementing using hyper's wrap_stream
    if let Some(content) = db_handle.get(&content_range) {
        Ok(Response::builder()
            .status(StatusCode::OK)
            .body(Body::from((*content).clone()))
            .unwrap())
    } else {
        Ok(Response::builder()
            .status(StatusCode::NOT_FOUND)
            .body(Body::empty())
            .unwrap())
    }
}

pub async fn run(hostname: &str, port: &str) -> crate::Result<()> {
    let ip_addr = hostname.parse().unwrap();
    let port_num = port.parse().unwrap();
    let server_socket = SocketAddr::new(ip_addr, port_num);

    let db = Db::new();

    let handler = make_service_fn(move |_| {
        let db_handle = db.clone();
        async move {
            Ok::<_, hyper::Error>(service_fn(move |req| {
                request_router(req, db_handle.clone())
            }))
        }
    });

    Server::bind(&server_socket).serve(handler).await?;

    Ok(())
}

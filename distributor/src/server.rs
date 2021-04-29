use std::io::SeekFrom;
use std::net::SocketAddr;

use hyper::http::header::CONTENT_RANGE;
use hyper::service::{make_service_fn, service_fn};
use hyper::{Body, Method, Request, Response, Server, StatusCode};
use hyper_staticfile::ResolveResult;
use tokio::io::{AsyncReadExt, AsyncSeekExt};

use crate::ContentRange;
use http::header;
use std::convert::TryInto;

async fn request_router(req: Request<Body>) -> crate::Result<Response<Body>> {
    dbg!(&req);
    match *req.method() {
        Method::GET => handle_get(req).await,
        _ => Ok(Response::new(Body::from("Unsupported Method"))),
    }
}

async fn handle_get(request: Request<Body>) -> crate::Result<Response<Body>> {
    // UNWRAP: previously validated
    let root = std::path::PathBuf::from(&std::env::var("CONTENT_ROOT").unwrap());
    let resolved_path = hyper_staticfile::resolve(&root, &request).await.unwrap();

    if let Some(content_range_value) = request.headers().get(CONTENT_RANGE) {
        let content_range = ContentRange::from(content_range_value);
        Ok(serve_partial_file(&request, resolved_path, content_range).await)
    } else {
        Ok(serve_file(&request, resolved_path))
    }
}

fn serve_file(request: &Request<Body>, resolved_path: ResolveResult) -> Response<Body> {
    hyper_staticfile::ResponseBuilder::new()
        .request(request)
        .build(resolved_path)
        .unwrap()
}

async fn serve_partial_file(
    request: &Request<Body>,
    resolved_path: ResolveResult,
    content_range: ContentRange,
) -> Response<Body> {
    match resolved_path {
        ResolveResult::Found(mut file, metadata, mime) => {
            // Add the seek call here
            file.seek(SeekFrom::Start(content_range.start().try_into().unwrap()))
                .await
                .unwrap();
            let mut buf = vec![0u8; content_range.length()];
            file.read_exact(&mut buf).await.unwrap();

            Response::builder()
                .status(StatusCode::PARTIAL_CONTENT)
                .body(Body::from(buf))
                .map(|mut r| {
                    let header_val =
                        header::HeaderValue::from_str(&mime.to_string()).expect("Invalid mimetype");
                    r.headers_mut().insert(header::CONTENT_TYPE, header_val);
                    r.headers_mut().insert(
                        header::CONTENT_RANGE,
                        request.headers().get(CONTENT_RANGE).unwrap().clone(),
                    );
                    r
                })
                .unwrap()
        }
        _ => Response::builder()
            .status(StatusCode::BAD_REQUEST)
            .body(Body::empty())
            .unwrap(),
    }
}

pub async fn run(hostname: &str, port: &str) -> crate::Result<()> {
    let ip_addr = hostname.parse().unwrap();
    let port_num = port.parse().unwrap();
    let server_socket = SocketAddr::new(ip_addr, port_num);
    let handler =
        make_service_fn(move |_| async move { Ok::<_, hyper::Error>(service_fn(request_router)) });

    Server::bind(&server_socket).serve(handler).await?;

    Ok(())
}

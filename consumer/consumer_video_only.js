const videoTag = document.getElementById("my_video");
const myMediaSource = new MediaSource();
const url = URL.createObjectURL(myMediaSource);
videoTag.src = url;

// 1. add source buffers
const videoSourceBuffer = myMediaSource
  .addSourceBuffer('video/mp4; codecs="avc1.64001e"');

// 2. download and add our video to the SourceBuffers
fetch("http://localhost:8000/video.mp4").then(function(response) {
  // The data has to be a JavaScript ArrayBuffer
  return response.arrayBuffer();
}).then(function(videoData) {
  videoSourceBuffer.appendBuffer(videoData);
});

/**
 * Fetch a video segment, and returns it as an ArrayBuffer, in a
 * Promise.
 * @param {string} url
 * @returns {Promise.<ArrayBuffer>}
 */
function fetchSegment(url) {
  return fetch(url).then(function(response) {
    return response.arrayBuffer();
  });
}

// fetch all video segments
fetchSegment("http://localhost:8000/video/segment0.mp4")
  .then(function(videoSegment0) {
    videoSourceBuffer.appendBuffer(videoSegment0);
  });

  .then(function() {
    return fetchSegment("http://localhost:8000/video/segment1.mp4");
  })
  .then(function(videoSegment1) {
    videoSourceBuffer.appendBuffer(videoSegment1);
  })

  .then(function() {
    return fetchSegment("http://localhost:8000/video/segment2.mp4");
  })
  .then(function(videoSegment2) {
    videoSourceBuffer.appendBuffer(videoSegment2);
  })

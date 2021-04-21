const videoTag = document.getElementById("my_video");
const myMediaSource = new MediaSource();
const url = URL.createObjectURL(myMediaSource);
videoTag.src = url;

// 1. add source buffers

const audioSourceBuffer = myMediaSource
  .addSourceBuffer('audio/mp4; codecs="mp4a.40.2"');
const videoSourceBuffer = myMediaSource
  .addSourceBuffer('video/mp4; codecs="avc1.64001e"');

// 2. download and add our audio/video to the SourceBuffers

// for the audio SourceBuffer
fetch("http://server.com/audio.mp4").then(function(response) {
  // The data has to be a JavaScript ArrayBuffer
  return response.arrayBuffer();
}).then(function(audioData) {
  audioSourceBuffer.appendBuffer(audioData);
});

// the same for the video SourceBuffer
fetch("http://server.com/video.mp4").then(function(response) {
  // The data has to be a JavaScript ArrayBuffer
  return response.arrayBuffer();
}).then(function(videoData) {
  videoSourceBuffer.appendBuffer(videoData);
});

/**
 * Fetch a video or an audio segment, and returns it as an ArrayBuffer, in a
 * Promise.
 * @param {string} url
 * @returns {Promise.<ArrayBuffer>}
 */
function fetchSegment(url) {
  return fetch(url).then(function(response) {
    return response.arrayBuffer();
  });
}

// fetching audio segments one after another (notice the URLs)
fetchSegment("http://server.com/audio/segment0.mp4")
  .then(function(audioSegment0) {
    audioSourceBuffer.appendBuffer(audioSegment0);
  })

  .then(function() {
    return fetchSegment("http://server.com/audio/segment1.mp4");
  })
  .then(function(audioSegment1) {
    audioSourceBuffer.appendBuffer(audioSegment1);
  })

  .then(function() {
    return fetchSegment("http://server.com/audio/segment2.mp4");
  })
  .then(function(audioSegment2) {
    audioSourceBuffer.appendBuffer(audioSegment2);
  })

// same thing for video segments
fetchSegment("http://server.com/video/segment0.mp4")
  .then(function(videoSegment0) {
    videoSourceBuffer.appendBuffer(videoSegment0);
  });

  .then(function() {
    return fetchSegment("http://server.com/video/segment1.mp4");
  })
  .then(function(videoSegment1) {
    videoSourceBuffer.appendBuffer(videoSegment1);
  })

  .then(function() {
    return fetchSegment("http://server.com/video/segment2.mp4");
  })
  .then(function(videoSegment2) {
    videoSourceBuffer.appendBuffer(videoSegment2);
  })

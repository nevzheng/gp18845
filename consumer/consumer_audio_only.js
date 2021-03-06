const videoTag = document.getElementById("my_video");
const myMediaSource = new MediaSource();
const url = URL.createObjectURL(myMediaSource);
videoTag.src = url;

// 1. add source buffers

const audioSourceBuffer = myMediaSource
  .addSourceBuffer('audio/mp4; codecs="mp4a.40.2"');

// 2. download and add our audio to the SourceBuffers

// for the audio SourceBuffer
fetch("http://localhost:8000/audio.mp4").then(function(response) {
  // The data has to be a JavaScript ArrayBuffer
  return response.arrayBuffer();
}).then(function(audioData) {
  audioSourceBuffer.appendBuffer(audioData);
});

/**
 * Fetch an audio segment, and returns it as an ArrayBuffer, in a
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
fetchSegment("http://localhost:8000/audio/segment0.mp4")
  .then(function(audioSegment0) {
    audioSourceBuffer.appendBuffer(audioSegment0);
  })

  .then(function() {
    return fetchSegment("http://localhost:8000/audio/segment1.mp4");
  })
  .then(function(audioSegment1) {
    audioSourceBuffer.appendBuffer(audioSegment1);
  })

  .then(function() {
    return fetchSegment("http://localhost:8000/audio/segment2.mp4");
  })
  .then(function(audioSegment2) {
    audioSourceBuffer.appendBuffer(audioSegment2);
  })


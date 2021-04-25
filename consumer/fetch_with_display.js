var videoTag = document.getElementById("my-video");
var myMediaSource = new MediaSource();
videoTag.src = URL.createObjectURL(myMediaSource);

// 1. add source buffers
myMediaSource.addEventListener("sourceopen", doit);

function doit(){

const audioSourceBuffer = myMediaSource
  .addSourceBuffer('audio/mp4; codecs="mp4a.40.2"');


const videoSourceBuffer = myMediaSource
  .addSourceBuffer('video/mp4; codecs="avc1.4d4028"');

// 2. download and add our audio/video to the SourceBuffers


// for the audio SourceBuffer
fetch("/segments/audio_and_video/tears-of-steel-battle-clip-medium-fixed.mp4").then(function(response) {
  // The data has to be a JavaScript ArrayBuffer
  return response.arrayBuffer();
}).then(function(audioData) {
  audioSourceBuffer.appendBuffer(audioData);
});
// the same for the video SourceBuffer
fetch("/segments/audio_and_video/tears-of-steel-battle-clip-medium-fixed.mp4").then(function(response) {
  // The data has to be a JavaScript ArrayBuffer
  return response.arrayBuffer();
}).then(function(videoData) {
  videoSourceBuffer.appendBuffer(videoData);
});

}

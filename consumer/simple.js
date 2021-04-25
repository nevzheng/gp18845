let videoTag = document.getElementById("my-video");
let myMediaSource = new MediaSource();
videoTag.src = URL.createObjectURL(myMediaSource);

myMediaSource.addEventListener("sourceopen", doit)

function fetchSegment(url) {
  return fetch(url).then(function(response) {
    return response.arrayBuffer();
  });
}

function doit(){
// 1. add source buffers
    // segment1
   const videoSourceBuffer = myMediaSource.addSourceBuffer('video/mp4; codecs="mp4a.40.2,avc1.64002a"'); 
    // tears of steel
//const videoSourceBuffer = myMediaSource.addSourceBuffer('video/mp4; codecs="avc1.4d4028,mp4a.40.2"');

// 2. download and add our audio/video to the SourceBuffers

fetchSegment("/segments/audio_and_video/segment1_fixed.mp4")
.then(function(videoSegment1) {
  videoSourceBuffer.appendBuffer(videoSegment1);
})
.then(function() {
    return fetchSegment("/segments/audio_and_video/segment2_fixed.mp4");
  })
  .then(function(videoSegment2) {
    videoSourceBuffer.appendBuffer(videoSegment2);
  })
}

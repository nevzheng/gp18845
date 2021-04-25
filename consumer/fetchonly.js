let videoTag = document.getElementById("my-video");                             
let myMediaSource = new MediaSource();                                          
videoTag.src = URL.createObjectURL(myMediaSource);

myMediaSource.addEventListener("sourceopen", pushToBuffer);

fileLimit = 1000;
currentRange = 0;
delta = 200;
fileURL = "/consumer/segments/audio_and_video/segment1.mp4"

function pushToBuffer(){
    while(currentRange < fileLimit){
        fetch(fileURL, {
            method: 'GET',
            headers: {
                "Content-Type": "video/mp4",
                "Content-Range": `bytes ${currentRange}-${currentRange+delta}/${fileLimit}`
            }
        })

        currentRange = currentRange + delta;
    }
}

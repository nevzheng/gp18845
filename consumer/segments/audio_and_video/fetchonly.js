let videoTag = document.getElementById("my-video");                             
let myMediaSource = new MediaSource();                                          
videoTag.src = URL.createObjectURL(myMediaSource);

myMediaSource.addEventListener("sourceopen", pushToBuffer);

fileLimit = 1000;
currentRange = 0;
delta = 200;

// change file url path here and below as needed by server
fileURL = "segment1.mp4";
fileCount = 1;

function pushToBuffer(){
    while(fileCount <= 4){
        fetch(fileURL, {
            method: 'GET',
            headers: {
                "Content-Type": "video/mp4",
            }
        })
        fileCount = fileCount + 1;
        fileURL = `segment${fileCount}.mp4`;
    }
}

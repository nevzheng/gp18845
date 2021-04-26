let videoTag = document.getElementById("my-video");                             
let myMediaSource = new MediaSource();                                          
videoTag.src = URL.createObjectURL(myMediaSource);

myMediaSource.addEventListener("sourceopen", pushToBuffer);

fileLimit = 1000;
currentRange = 0;
delta = 200;

// change file url path here and below as needed by server
fileURL = "/test_files/segment1.mp4";
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
        if(fileCount === 2){
            fileURL = "/test_files/segment2.mp4";
        }
        else if(fileCount === 3){
            fileURL = "/test_files/segment3.mp4";
        }
        else if(fileCount === 4){
            fileURL = "/test_files/segment4.mp4";
        }
    }
}

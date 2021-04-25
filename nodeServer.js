/* if you get dependencies error with node because of express, run:
 * > npm install express
 * then they will go away.
 * 
 * To run the server do:
 * > node nodeServer.js
 *
 * the server will serve any files inside of: consumer/segments/audio_and_video
 * the relevant URL is:
 * http://localhost:3000/test.html
 * which will serve the html file that runs the fetchonly.js script and downloads
 * all of the segments of the mp4 file that get streamed
 *
 */

const express = require('express');
const app = express();
const port = 3000;

app.use(express.static('consumer/segments/audio_and_video'));

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
});

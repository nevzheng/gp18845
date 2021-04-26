/* if you get dependencies error with node because of express, run:
 * > npm install express
 * then they will go away.
 * 
 * To run the server do:
 * > node nodeServer.js
 *
 * the 4 files to be served by this server can be served by running this server
 * and going to any of the following
 * http://localhost:3000/segment1.mp4
 * http://localhost:3000/segment2.mp4
 * http://localhost:3000/segment3.mp4
 * http://localhost:3000/segment4.mp4
 *
 */

const express = require('express');
const app = express();
const port = 3000;

app.use(express.static('consumer/segments/audio_and_video'));

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
});

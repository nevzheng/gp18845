/* if you get dependencies error with node because of express, run:
 * > npm install express
 * then they will go away.
 * 
 * To run the server do:
 * > node nodeServer.js
 *
 * to run the consumer go to:
 * http://localhost:3000/test.html
 */

const express = require('express');
const app = express();
const port = 3000;

app.use(express.static('node_test_files'));

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
});

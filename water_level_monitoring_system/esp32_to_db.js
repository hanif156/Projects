const app = require("express")();
const mysql = require('mysql');

// In order to read and parse the JSON POST request
var bodyParser = require('body-parser');
app.use(bodyParser.json());

// MySQL database connection
var db = mysql.createConnection({
  host: 'Your_host_IP',
  user: 'Your_username',
  password: 'Your_Password',
  database: 'Your_scheme'
  // table: 'table_v1'
})
db.connect(function(err) {
  if (err) throw err;
  console.log("Connected!");
  let millis = Date.now();
  console.log("Current time in millis: ", millis);
});

// Start listening to specific port
app.listen(8085, () => console.log("Listening to port 8085."));

// REST GET request ------------------------------------------------
app.get('/get', (req, res) => { 
  res.send("Get function was activated ...");

  // Read the data from MySQL table
  console.log("MySQL table data:");
  let sql_get = 'SELECT * FROM personal_information';
  db.query(sql_get,(err, result) => {
    if (err) throw err;
    console.log(result);
  });
});

//REST POST request -----------------------------------------------
app.post('/post', (req, res) => { 
  res.send("Post function was activated ...");

  let post_data = {water_level_data : req.body.water_level_data};
  // Send the data block to MySQL table
  let sql_post = 'INSERT INTO water_level_sensor SET ?';
  db.query(sql_post, post_data, (err, result) => {
    if (err) throw err;
  });
});
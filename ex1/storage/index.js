const express = require("express");
const axios = require("axios");
const fs = require("fs");
const app = express();

app.use(express.json());

const DEBUG = 1;
const PORT = process.env.PORT || 8080;
const FILENAME = "./logs/logs.txt";

// logging stream
var logStream = fs.createWriteStream(FILENAME, { flags: "a+" });

app.post("/log", async (req, res) => {
  if (DEBUG) {
    console.log("Updating file...");
    console.log(req.body);
  }
  logStream.write(`${req.body.data}\n`);
  res.send();
});

app.get("/log", async (req, res) => {
  if (DEBUG) {
    console.log("Searching file...");
  }

  // read file and respond
  fs.readFile(FILENAME, "utf-8", function (err, data) {
    if (err) {
      console.log("Error reading storage: ", err);
      res.status(404).send("Error: File not found");
      return; // do this better
    }
    console.log("Found ", FILENAME);
    res.type("text/plain").send(data);
  });
});

// 501 handler
app.use((req, res) => {
  res.status(501).type("text/plain").send("501 Not Implemented");
});

app.listen(PORT, () => {
  console.log("Storage running on port: ", PORT);
});

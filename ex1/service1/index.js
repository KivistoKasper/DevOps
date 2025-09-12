const express = require("express");
const axios = require("axios");
const app = express();
const PORT = 8199;

const DEBUG = 1;

app.get("/status", async (req, res) => {
  // needed vriables
  const timestamp = new Date(Date.now()).toISOString();
  const uptime = process.uptime() / 3600;
  const uptimeHours = Math.round(uptime * 10) / 10;

  // constructing message
  const msg = `--SERVICE 1-- ${timestamp}: uptime ${uptimeHours} hours, free disk in root: <X> MBytes`;
  var msg2 = "";
  // proxy the message to service 2
  await axios
    .get("http://localhost:9191/status", {
      headers: {
        Accept: "*/*",
      },
    })
    .then((res) => {
      if (false) {
        console.log("service2: ", res.data);
      }
      msg2 = res.data;
    })
    .catch((error) => {
      console.log("Error: axios error:", error.message);
    });

  // sending and logging
  if (DEBUG) {
    console.log("service1: ", msg);
    console.log("service2: ", msg2);
  }

  res.send(`${msg}\n${msg2}`);
});

app.listen(PORT, () => {
  console.log("Server running on port: ", PORT);
});

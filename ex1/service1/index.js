const express = require("express");
const axios = require("axios");
const app = express();

const DEBUG = 1;
const PORT = process.env.PORT || 8199;
const URL = process.env.URL || "localhost";
const S2PORT = process.env.S2PORT || 9191;

function constructMessage() {
  // needed variables
  const timestamp = new Date(Date.now()).toISOString();
  const uptime = process.uptime() / 3600;
  const uptimeHours = Math.round(uptime * 10) / 10;

  // constructing message
  const msg = `--SERVICE 1-- ${timestamp}: uptime ${uptimeHours} hours, free disk in root: <X> MBytes`;
  return msg;
}

app.get("/status", async (req, res) => {
  var msg2 = "";

  // proxy the message to service 2
  const proxyUrl = `http://${URL}:${S2PORT}/status`;
  await axios
    .get(proxyUrl, {
      headers: {
        Accept: "*/*",
      },
    })
    .then((res) => {
      msg2 = res.data;
    })
    .catch((error) => {
      console.log("Error: Axios error:", error.message);
    });

  const msg = constructMessage();

  await axios
    .post("http://localhost:8080/log", { data: msg })
    .then((res) => {
      console.log("Writing done");
    })
    .catch((error) => {
      console.log("Error: Axios Write Error ", error);
    });
  // sending and logging
  if (DEBUG) {
    console.log("service1: ", msg);
    console.log("service2: ", msg2);
  }

  res.send(`${msg}\n${msg2}`);
});

app.get("/log", async (req, res) => {
  var logs = "";
  // proxy the request to storage
  await axios
    .get("http://localhost:8080/log")
    .then((res) => {
      //console.log("Message from storgae: ", res.data);
      logs = res.data;
    })
    .catch((error) => {
      console.log("Error: Axios error:", error.message);
    });

  res.send(logs);
});

app.listen(PORT, () => {
  console.log("Service1 running on port: ", PORT);
});

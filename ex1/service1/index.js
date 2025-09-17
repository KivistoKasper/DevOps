const express = require("express");
const axios = require("axios");
const app = express();
const disk = require("diskusage");

const DEBUG = 1;
const PORT = process.env.PORT || 8199;
const S2_URL = process.env.S2_URL || "localhost";
const S2_PORT = process.env.S2_PORT || 9191;
const STORAGE_URL = process.env.STORAGE_URL || "localhost";
const STORAGE_PORT = process.env.STORAGE_PORT || 8080;

const S2proxyUrl = `http://${S2_URL}:${S2_PORT}/status`;
const storageProxyUrl = `http://${STORAGE_URL}:${STORAGE_PORT}/log`;
console.log(`WHOLE URLS: \n${S2proxyUrl}\n${storageProxyUrl}`);

function constructMessage() {
  // needed variables
  try {
    const timestamp = new Date(Date.now()).toISOString();
    const uptime = process.uptime() / 3600;
    const uptimeHours = Math.round(uptime * 10) / 10;
    const { available, free, total } = disk.checkSync("/");
    const freeDiskMB = Math.floor(free / (1024 * 1024));

    // constructing message
    const msg = `--SERVICE 1-- ${timestamp}: uptime ${uptimeHours} hours, free disk in root: ${freeDiskMB} MBytes`;
    return msg;
  } catch (err) {
    console.log("Error: error on message variables.");
  }
}

app.get("/status", async (req, res) => {
  var msg2 = "";

  // proxy the message to service 2
  await axios
    .get(S2proxyUrl, {
      headers: {
        Accept: "*/*",
      },
    })
    .then((res) => {
      msg2 = res.data;
    })
    .catch((error) => {
      console.log("Error: Axios Service 2 error:", error.message);
    });

  const msg = constructMessage();

  // proxy the status to storage
  await axios
    .post(storageProxyUrl, { data: msg })
    .then((res) => {
      //console.log("Writing done");
    })
    .catch((error) => {
      console.log("Error: Axios Write Error ", error);
    });
  // sending and logging
  if (DEBUG) {
    console.log("service1: ", msg);
    console.log("service2: ", msg2);
  }

  res.type("text/plain").send(`${msg}\n${msg2}`);
});

app.get("/log", async (req, res) => {
  var logs = "";
  // proxy the request to storage
  await axios
    .get(storageProxyUrl)
    .then((res) => {
      //console.log("Message from storgae: ", res.data);
      logs = res.data;
    })
    .catch((error) => {
      console.log("Error: Axios Get Log error:", error.message);
    });

  res.send(logs);
});

app.listen(PORT, () => {
  console.log("Service1 running on port: ", PORT);
});

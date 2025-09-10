const express = require("express");
const app = express();
const PORT = 8199;

app.get("/status", async (req, res) => {
  // needed vriables
  const timestamp = new Date(Date.now()).toISOString();
  const uptime = process.uptime() / 360;
  const uptimeHours = Math.round(uptime * 10) / 10;

  // constructing message
  const msg = `${timestamp}: uptime ${uptimeHours} hours, free disk in root: <X> MBytes`;

  // sending and logging
  console.log(msg);
  res.send(`${msg}\n`);
});

app.listen(PORT, () => {
  console.log("Server running on port: ", PORT);
});

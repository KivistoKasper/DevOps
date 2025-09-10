const express = require("express");
const app = express();
const PORT = 8199;

app.get("/status", async (req, res) => {
  const timestamp = new Date(Date.now()).toISOString();
  const msg = `${timestamp}: uptime <X> hours, free disk in root: <X> MBytes`;

  console.log(msg);
  res.send(`${msg}\n`);
});

app.listen(PORT, () => {
  console.log("Server running on port: ", PORT);
});

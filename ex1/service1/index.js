const express = require("express");
const app = express();
const PORT = 8199;

app.get("/status", async (req, res) => {
  res.send("This is the status of service1!\n");
});

app.listen(PORT, () => {
  console.log("Server running on port: ", PORT);
});

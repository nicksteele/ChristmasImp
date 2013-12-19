server.log("Agent Active - Control the Tree at " + http.agenturl() + "?cmd=#&r=#&g=#&b=#");

function httpHandler(req, resp) {
  // Check if the variable led was passed into the query
  
  local color = {
        red = 0,
        green = 0,
        blue = 0
    }
  
  if ("cmd" in req.query) {
    server.log("Imp Received command to " + req.query["cmd"])
    device.send("setTreeAction", req.query["cmd"]);
  } else {
      server.log("No Command");
  }
  
  if ("r" in req.query && "g" in req.query && "b" in req.query){
      server.log("Imp Requested to change color to [" + req.query["r"] + "," + req.query["g"] + "," + req.query["b"] + "]");
      color.red = req.query["r"].tointeger();
      color.green = req.query["g"].tointeger();
      color.blue = req.query["b"].tointeger();
      device.send("setTreeColor", color);
  }
  // send a response back to whoever made the request
  resp.send(200, "OK");
}

http.onrequest(httpHandler);

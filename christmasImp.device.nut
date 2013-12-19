//The Grand St. Xmas Tree LED Extraveganza 
server.log("Device Active")

arduino <- hardware.uart57
arduino1 <- hardware.uart1289

function arduinoData() {//See if the Command/Color was successful
    local b = arduino.read();
     while(b != -1) {
        local state = "Unknown";
        if (b == 0x10) state = "Success";
        if (b == 0x01) state = "Something Derped Hard";
        server.log("Command Execution: " + state);
        b = arduino.read();
     }
}

function setTreeColor(color){
    server.log("Color being changed to " + color.red + "," + color.green + "," + color.blue);
    local colorString = color.red + "," + color.green + "," + color.blue;
    arduino.write("Color:" + colorString + "\n");
    //arduino1.write(0x10); //if we want/know how to use triggers/interrupts
}

agent.on("setTreeAction" function(action) {
    if(action=="none") {
        server.log("no cmd");
    } else {
        server.log("Command to " + action + " being sent to tree");
        if(action == "ChangeSecondary"){
            arduino.write("Command:" + action + "\n");
            imp.sleep(4);
        } else {
            arduino.write("Command:" + action + "\n");
        }
        //arduino1.write(0x10); //see line 22
    }
});

agent.on("setTreeColor",setTreeColor);

arduino.configure(2400, 8, PARITY_NONE, 1, NO_CTSRTS, arduinoData);
arduino1.configure(2400, 8, PARITY_NONE, 1, NO_CTSRTS, arduinoData);

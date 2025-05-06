//SKETCH FOR BLUETOOTH MASTER-SLAVE CONFIGURATION

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}


// Bluetooth activation commands
// first AT - if it's OK - then it means the bletooth is in attention mode and is connected to the aruino
// AT+ROLE=0 - to configure the bluetooth module to slave (will not try to connect to any bluetooth module on its own)
// AT+ADDR? - to know the MAC address of the bluetooth device - this is useful to eventually connect the other bluetooth as a master to this one
// slave bluetooth address = +ADDR:98D3:71:F699E3

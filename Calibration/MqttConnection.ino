void mqttConnection()
{
  const char *broker = "mqtt.flespi.io";
  int port = 1883;
  mqttClient.setId("clientId");
  mqttClient.setUsernamePassword("Mr7DlDWxIjdRE412ku8uxCnxGeZnSISvZQWbjHyPOBwiun8EH6W2x7q1adcSLVbB", "password");
   if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }else {
  Serial.println("connected");
    lcd.setCursor(0,1);
    lcd.print("MQTT Connected");
    delay(1500);
  }
  
}
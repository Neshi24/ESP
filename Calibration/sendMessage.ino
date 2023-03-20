void sendMessage(int i)
{
    //const char *topic = "topic/this";
    mqttClient.poll();
    mqttClient.beginMessage("ESP/Publish");
    mqttClient.print(i);
    mqttClient.endMessage();
    Serial.println("sent to broker");
    sendingInterval = 0;
}
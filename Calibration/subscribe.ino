void subscribe()
{
   // set the message receive callback
  mqttClient.onMessage(onMqttMessage);
  // subscribe to a topic
  mqttClient.subscribe("APP/Publish");
  mqttClient.subscribe("APP/Startup");
}
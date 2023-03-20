void onMqttMessage(int messageSize) {
  while (mqttClient.available()) {
    String str = mqttClient.readString();
     if (str == "Hello"){
     sendMessage(i);
     }
     else if (str.toFloat() > 0){
      amountOfFood = str.toFloat();
     }
  }
}
package com.huy.smartlock_server.service;

import org.eclipse.paho.client.mqttv3.*;
import org.springframework.stereotype.Service;

@Service
public class MqttService {
    private static final String BROKER_URL = "tcp://broker.emqx.io:1883";
    private static final String CLIENT_ID = "Java_Backend_Commander_" + System.currentTimeMillis();

    public void sendUnlockCommand(String topic) {
        try {
            MqttClient client = new MqttClient(BROKER_URL, CLIENT_ID);
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            
            client.connect(options);
            
            // Gói tin chữ "OPEN"
            MqttMessage message = new MqttMessage("OPEN".getBytes());
            message.setQos(1); 
            
            // Bóp cò phát sóng
            client.publish(topic, message);
            System.out.println("[MQTT] Command sent to topic: " + topic);
            
            client.disconnect();
        } catch (MqttException e) {
            System.err.println("[MQTT ERROR] Command failed: " + e.getMessage());
        }
    }
    public void sendAdminOverrideCommand(String topic, String actionCommand) {
        try {
            MqttClient client = new MqttClient(BROKER_URL, CLIENT_ID + "_admin"); 
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            
            client.connect(options);
            
            MqttMessage message = new MqttMessage(actionCommand.getBytes());
            message.setQos(1); 
            

            client.publish(topic, message);
            System.out.println("[MQTT] Admin override executed. Command: [" + actionCommand + "] sent to topic: " + topic);
            
            client.disconnect();
        } catch (MqttException e) {
            System.err.println("[MQTT ERROR] Admin override failed: " + e.getMessage());
        }
    }
}
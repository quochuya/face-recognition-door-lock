package com.huy.smartlock_server.service;

import org.eclipse.paho.client.mqttv3.*;
import org.springframework.stereotype.Service;

@Service
public class MqttService {
    // Trạm trung chuyển đám mây (Giống hệt bên ESP32)
    private static final String BROKER_URL = "tcp://broker.emqx.io:1883";
    private static final String CLIENT_ID = "Java_Backend_Commander";

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
            System.out.println("[MQTT] Đã bắn lệnh 'OPEN' vào kênh: " + topic);
            
            client.disconnect();
        } catch (MqttException e) {
            System.err.println("[MQTT ERROR] Lỗi phát sóng: " + e.getMessage());
        }
    }
}
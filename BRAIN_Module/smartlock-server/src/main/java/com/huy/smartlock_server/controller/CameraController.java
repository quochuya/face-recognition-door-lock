package com.huy.smartlock_server.controller;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;

import java.util.HashMap;
import java.util.Map;

@RestController
@RequestMapping("/api/camera")
public class CameraController {


    @PostMapping("/start")
    public ResponseEntity<String> startCamera(@RequestBody Map<String, Object> payload) {
        try {
            int camIndex = (int) payload.get("cam_index");
            String camName = (String) payload.get("cam_name");
            
            // 1. LẤY THÊM SLOT INDEX TỪ WEB GỬI LÊN
            int slotIndex = (int) payload.get("slot_index");
            
            System.out.println("[COMMAND] Request camera: " + camName + " | Slot: " + slotIndex);

            Map<String, Object> cppPayload = new HashMap<>();
            cppPayload.put("cam_index", camIndex);
            cppPayload.put("cam_name", camName);
            cppPayload.put("slot_index", slotIndex); 

            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/add_cam"; 
            
            restTemplate.postForEntity(cppEndpoint, cppPayload, String.class);

            return ResponseEntity.ok("{\"status\": \"success\", \"message\": \"Camera is started!\"}");
            
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\": \"error\", \"message\": \"C++ Node is offline\"}");
        }
    }


    @PostMapping("/stop")
    public ResponseEntity<String> stopCamera(@RequestBody Map<String, Object> payload) {
        try {
            int slotIndex = (int) payload.get("slot_index");
            System.out.println("[COMMAND] Request to stop camera at slot: " + slotIndex);
            
            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/stop_cam"; 
            

            restTemplate.postForEntity(cppEndpoint, payload, String.class);

            return ResponseEntity.ok("{\"status\": \"success\", \"message\": \"Camera is stopped!\"}");
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\": \"error\", \"message\": \"C++ Node is offline\"}");
        }
    }

    @GetMapping("/check_face")
    public ResponseEntity<String> checkFace() {
        try {
            System.out.println("[API] Web yeu cau kiem tra khuon mat truoc ong kinh...");
            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/check_face"; 
            
            // Gọi GET xuống C++ và lấy phản hồi trả thẳng về Web
            ResponseEntity<String> cppResponse = restTemplate.getForEntity(cppEndpoint, String.class);
            return ResponseEntity.ok(cppResponse.getBody());
            
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\": \"error\", \"message\": \"C++ Node is offline\"}");
        }
    }


    @PostMapping("/collect")
    public ResponseEntity<String> collectFaceData(@RequestBody Map<String, Object> payload) {
        try {
            System.out.println("[API] Web ra lenh C++ chup 50 anh cho ID: " + payload.get("user_id"));
            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/collect_face_data"; 
            
            // Chuyển tiếp payload (chứa user_id, full_name, role) xuống C++
            ResponseEntity<String> cppResponse = restTemplate.postForEntity(cppEndpoint, payload, String.class);
            return ResponseEntity.ok(cppResponse.getBody());
            
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\": \"error\", \"message\": \"C++ Node is offline\"}");
        }
    }


    @PostMapping("/train")
    public ResponseEntity<String> trainModel() {
        try {
            System.out.println("[API] Web ra lenh C++ huan luyen lai AI!");
            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/train_model"; 
            
            // Tạo request rỗng để kích hoạt Train
            Map<String, Object> emptyPayload = new HashMap<>();
            ResponseEntity<String> cppResponse = restTemplate.postForEntity(cppEndpoint, emptyPayload, String.class);
            
            return ResponseEntity.ok(cppResponse.getBody());
            
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\": \"error\", \"message\": \"C++ Node is offline\"}");
        }
    }
}
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
            // 1. Nhận lệnh từ Web Dashboard HTML
            int camIndex = (int) payload.get("cam_index");
            String camName = (String) payload.get("cam_name");
            System.out.println("[COMMAND] Request camera: " + camName);

            // 2. Đóng gói quà JSON để mang sang tặng C++
            Map<String, Object> cppPayload = new HashMap<>();
            cppPayload.put("cam_index", camIndex);
            cppPayload.put("cam_name", camName);

            // 3. Đổi cổng sang POST và gọi đúng tên /add_cam
            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/add_cam"; 
            
            // Dùng postForEntity thay vì getForEntity
            ResponseEntity<String> cppResponse = restTemplate.postForEntity(cppEndpoint, cppPayload, String.class);

            return ResponseEntity.ok("SUCCESS: Camera đã được đánh thức!");
            
        } catch (Exception e) {
            System.err.println("[ERROR] Đứt cáp sang C++: " + e.getMessage());
            return ResponseEntity.status(500).body("FAILED: C++ Node is offline");
        }
    }
    @PostMapping("/stop")
    public ResponseEntity<String> stopCamera(@RequestBody Map<String, Object> payload) {
        try {
            System.out.println("[COMMAND] Web yêu cầu TẮT camera!");
            RestTemplate restTemplate = new RestTemplate();
            
            // Bắn lệnh sang cổng 8081 của C++ để dập cầu dao
            String cppEndpoint = "http://localhost:8081/stop_cam"; 
            restTemplate.postForEntity(cppEndpoint, payload, String.class);

            return ResponseEntity.ok("SUCCESS: Camera đã tắt!");
        } catch (Exception e) {
            System.err.println("[ERROR] Lỗi dập cầu dao C++: " + e.getMessage());
            return ResponseEntity.status(500).body("FAILED");
        }
    }
}
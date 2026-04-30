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
            System.out.println("[COMMAND] Request camera: " + camName);

            Map<String, Object> cppPayload = new HashMap<>();
            cppPayload.put("cam_index", camIndex);
            cppPayload.put("cam_name", camName);

            RestTemplate restTemplate = new RestTemplate();
            String cppEndpoint = "http://localhost:8081/add_cam"; 
            
            ResponseEntity<String> cppResponse = restTemplate.postForEntity(cppEndpoint, cppPayload, String.class);

            return ResponseEntity.ok("SUCCESS: Camera is started!");
            
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("FAILED: C++ Node is offline");
        }
    }
    @PostMapping("/stop")
    public ResponseEntity<String> stopCamera(@RequestBody Map<String, Object> payload) {
        try {
            System.out.println("[COMMAND] Request to stop camera!");
            RestTemplate restTemplate = new RestTemplate();
            
            String cppEndpoint = "http://localhost:8081/stop_cam"; 
            restTemplate.postForEntity(cppEndpoint, payload, String.class);

            return ResponseEntity.ok("SUCCESS: Camera is stopped!");
        } catch (Exception e) {
            System.err.println("[ERROR] C++ Node is offline: " + e.getMessage());
            return ResponseEntity.status(500).body("FAILED: C++ Node is offline");
        }
    }
}
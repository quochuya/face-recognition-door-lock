package com.huy.smartlock_server.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import com.huy.smartlock_server.entity.AccessLog;
import com.huy.smartlock_server.repository.AccessLogRepository;
import com.huy.smartlock_server.service.MqttService;

import java.util.Map;

@RestController
@RequestMapping("/api")
public class DoorController {

    @Autowired
    private MqttService mqttService;

    @Autowired
    private AccessLogRepository logRepository; 

    @PostMapping("/override")
    public ResponseEntity<?> overrideDoor(@RequestBody Map<String, String> request) {
        String action = request.get("action");

        String mqttCommand = action.equals("OPEN") ? "FORCE_OPEN" : "FORCE_CLOSE";

        mqttService.sendAdminOverrideCommand("huy_smartlock/locks/main", mqttCommand);

        AccessLog log = new AccessLog();
        log.setUserName("ADMIN_OVERRIDE");       
        log.setStatus("TACTICAL_" + action);      
        logRepository.save(log); // Đẩy xuống MySQL

        return ResponseEntity.ok().build();
    }
}

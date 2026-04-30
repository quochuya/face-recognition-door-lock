package com.huy.smartlock_server.service;

import com.huy.smartlock_server.entity.AccessLog;
import com.huy.smartlock_server.entity.User;
import com.huy.smartlock_server.repository.AccessLogRepository;
import com.huy.smartlock_server.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.Optional;

@Service
public class UnlockService {

    @Autowired
    private UserRepository userRepository;

    @Autowired
    private AccessLogRepository accessLogRepository;

    @Autowired
    private MqttService mqttService;

    public ResponseEntity<String> processUnlock(String cameraName, int userId, double distance) {
        

        // Kiểm tra danh tính: Truy vấn thông tin người dùng từ Database
        Optional<User> userOptional = userRepository.findById(userId);
        
        if (userOptional.isEmpty()) {
            saveLog(cameraName, userId, "Unknown", distance, "DENIED_UNKNOWN_USER");
            return ResponseEntity.status(HttpStatus.FORBIDDEN).body("DENIED: ID không tồn tại trong hệ thống.");
        }

        User user = userOptional.get();

        //Kiểm tra giờ giới nghiêm
        LocalTime now = LocalTime.now();
        boolean isNightTime = now.isAfter(LocalTime.of(22, 59)) || now.isBefore(LocalTime.of(6, 0));

        if (isNightTime) {
            if (!"ADMIN".equalsIgnoreCase(user.getRole())) {
                System.out.println("[WARNING]: " + user.getFullName() + "try to unlock during curfew hours. Access denied.");
                saveLog(cameraName, user.getId(), user.getFullName(), distance, "DENIED_CURFEW");
                return ResponseEntity.status(HttpStatus.FORBIDDEN).body("DENIED: Only ADMIN can unlock during curfew hours.");
            }
        }
        System.out.println("[SUCCESS] Open for : " + user.getFullName() + " |Distance: " + distance);
        saveLog(cameraName, user.getId(), user.getFullName(), distance, "ALLOWED");
        mqttService.sendUnlockCommand("huy_smartlock/locks/main");

        return ResponseEntity.ok("ALLOWED: " + user.getFullName());
    }

    private void saveLog(String camName, int userId, String userName, double distance, String status) {
        AccessLog log = new AccessLog();
        log.setCameraName(camName);
        log.setUserId(userId);
        log.setUserName(userName);
        log.setConfidence(distance);
        log.setStatus(status);
        log.setTimestamp(LocalDateTime.now());
        
        accessLogRepository.save(log);
    }
}
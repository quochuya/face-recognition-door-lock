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

    
        Optional<User> userOptional = userRepository.findById(userId);
        
        if (userOptional.isEmpty()) {
       
            System.out.println("[SECURITY ALERT]: Phat hien ID " + userId + " da bi vo hieu hoa dang co gang mo cua!");
            saveLog(cameraName, userId, "Người dùng đã bị xóa", distance, "DENIED_DEACTIVATED");
            return ResponseEntity.status(HttpStatus.FORBIDDEN).body("DENIED: Tài khoản đã bị vô hiệu hóa hoặc không tồn tại.");
        }

        User user = userOptional.get();

        // Bước 2: Kiểm tra giờ giới nghiêm (23:00 - 06:00)
        LocalTime now = LocalTime.now();
        boolean isNightTime = now.isAfter(LocalTime.of(22, 59)) || now.isBefore(LocalTime.of(6, 0));

        if (isNightTime) {
            if (!"ADMIN".equalsIgnoreCase(user.getRole())) {
                System.out.println("[WARNING]: " + user.getFullName() + " try to unlock during curfew hours. Access denied.");
                saveLog(cameraName, user.getId(), user.getFullName(), distance, "DENIED_CURFEW");
                return ResponseEntity.status(HttpStatus.FORBIDDEN).body("DENIED: Only ADMIN can unlock during curfew hours.");
            }
        }

        // Bước 3: Cho phép mở cửa
        System.out.println("[SUCCESS] Open for: " + user.getFullName() + " | Distance: " + distance);
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
        log.setTimestamp(LocalDateTime.now());
        log.setStatus(status);
        
        accessLogRepository.save(log);
    }
}
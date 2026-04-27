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

    public ResponseEntity<String> processUnlock(String cameraName, int userId, double distance) {
        
        // 1. Kiểm tra danh tính: Truy vấn thông tin người dùng từ Database
        Optional<User> userOptional = userRepository.findById(userId);
        
        if (userOptional.isEmpty()) {
            // C++ gửi ID không tồn tại trong DB Web
            saveLog(cameraName, userId, "Unknown", distance, "DENIED_UNKNOWN_USER");
            return ResponseEntity.status(HttpStatus.FORBIDDEN).body("DENIED: ID không tồn tại trong hệ thống.");
        }

        User user = userOptional.get();

        // 2. CHỐT CHẶN AN NINH: Kiểm tra giờ giới nghiêm
        LocalTime now = LocalTime.now();
        // Giới nghiêm từ 23:00 đêm đến 05:59 sáng
        boolean isNightTime = now.isAfter(LocalTime.of(22, 59)) || now.isBefore(LocalTime.of(6, 0));

        if (isNightTime) {
            // Ban đêm: Chỉ ADMIN mới được mở
            if (!"ADMIN".equalsIgnoreCase(user.getRole())) {
                System.out.println("[WARNING] An ninh: " + user.getFullName() + " cố gắng mở cửa trong giờ giới nghiêm!");
                saveLog(cameraName, user.getId(), user.getFullName(), distance, "DENIED_CURFEW");
                return ResponseEntity.status(HttpStatus.FORBIDDEN).body("DENIED: Đang trong giờ giới nghiêm. Chỉ Admin mới được mở.");
            }
        }

        // 3. Hợp lệ hoàn toàn -> Cấp quyền mở cửa
        System.out.println("[SUCCESS] Mở cửa cho: " + user.getFullName() + " | Sai số: " + distance);
        saveLog(cameraName, user.getId(), user.getFullName(), distance, "ALLOWED");
        
        // (Gợi ý mở rộng: Gọi API MQTT xuống mạch ESP32/openHAB để mở khóa thực tế ở đây)

        return ResponseEntity.ok("ALLOWED: " + user.getFullName());
    }

    // Hàm phụ trợ ghi Log ra Database
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
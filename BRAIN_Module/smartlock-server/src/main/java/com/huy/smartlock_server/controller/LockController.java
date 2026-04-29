package com.huy.smartlock_server.controller;

import com.huy.smartlock_server.dto.UnlockRequestDto;
import com.huy.smartlock_server.repository.AccessLogRepository;
import com.huy.smartlock_server.service.UnlockService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api")
public class LockController {

    @Autowired
    private UnlockService unlockService;

    // Endpoint nhận tín hiệu từ mô-đun C++
    @PostMapping("/detect")
    public ResponseEntity<String> requestUnlock(@RequestBody UnlockRequestDto request) {
        
        System.out.println("\n[API] Sign from camera: " + request.getCameraName() 
                         + " | ID: " + request.getUserId() + " | Sai số: " + request.getDistance());

        // Đẩy toàn bộ dữ liệu xuống Service xử lý và trả kết quả thẳng về C++
        return unlockService.processUnlock(
                request.getCameraName(),
                request.getUserId(),
                request.getDistance()
        );
    }
    @Autowired
    private AccessLogRepository logRepository; // Gọi cái Repository bạn đã dùng để lưu log lúc nãy

    @GetMapping("/logs")
    public ResponseEntity<?> getAllLogs() {
        // Lấy tất cả lịch sử, có thể thêm Sort để thằng nào mới nhất hiện lên trên
        return ResponseEntity.ok(logRepository.findAll()); 
    }
}
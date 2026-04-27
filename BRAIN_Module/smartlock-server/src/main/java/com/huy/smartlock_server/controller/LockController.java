package com.huy.smartlock_server.controller;

import com.huy.smartlock_server.dto.UnlockRequestDto;
import com.huy.smartlock_server.service.UnlockService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api")
public class LockController {

    // Tiêm (Inject) Service vào Controller
    @Autowired
    private UnlockService unlockService;

    // Endpoint nhận tín hiệu từ mô-đun C++
    @PostMapping("/unlock")
    public ResponseEntity<String> requestUnlock(@RequestBody UnlockRequestDto request) {
        
        System.out.println("\n[API] Nhận tín hiệu từ Cam: " + request.getCameraName() 
                         + " | ID: " + request.getUserId() + " | Sai số: " + request.getDistance());

        // Đẩy toàn bộ dữ liệu xuống Service xử lý và trả kết quả thẳng về C++
        return unlockService.processUnlock(
                request.getCameraName(),
                request.getUserId(),
                request.getDistance()
        );
    }
}
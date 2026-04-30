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

    @PostMapping("/detect")
    public ResponseEntity<String> requestUnlock(@RequestBody UnlockRequestDto request) {
        
        System.out.println("\n[API] Sign from camera: " + request.getCameraName() 
                         + " | ID: " + request.getUserId() + " | Distance: " + request.getDistance());
        return unlockService.processUnlock(
                request.getCameraName(),
                request.getUserId(),
                request.getDistance()
        );
    }
}
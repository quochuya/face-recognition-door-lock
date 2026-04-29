package com.huy.smartlock_server.controller;

import com.huy.smartlock_server.entity.AccessLog;
import com.huy.smartlock_server.repository.AccessLogRepository; 
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api")
public class LogController {

    // @Autowired
    // private AccessLogRepository accessLogRepository; // Đảm bảo tên file Repository của bạn giống thế này

    // @GetMapping("/logs")
    // public ResponseEntity<Iterable<AccessLog>> getAllLogs() {

    //     return ResponseEntity.ok(accessLogRepository.findAll());
    // }
}
package com.huy.smartlock_server.controller;

import com.huy.smartlock_server.repository.AccessLogRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.data.domain.Sort;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api")
public class LogController {

    @Autowired
    private AccessLogRepository accessLogRepository;

    @GetMapping("/logs")
    public ResponseEntity<?> getAllLogs(
            @RequestParam(defaultValue = "0") int page,  
            @RequestParam(defaultValue = "20") int size    
    ) {
        Pageable pageable = PageRequest.of(page, size, Sort.by("timestamp").descending());
        
        Page<?> logPage = accessLogRepository.findAll(pageable);
        
        return ResponseEntity.ok(logPage);
    }
}
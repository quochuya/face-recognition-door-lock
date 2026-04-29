package com.huy.smartlock_server.controller;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.huy.smartlock_server.entity.User;
import com.huy.smartlock_server.repository.UserRepository;

@RestController
@RequestMapping("/api")
public class UserController {

    @Autowired
    private UserRepository userRepository; 

    @GetMapping("/users")
    public ResponseEntity<Iterable<User>> getAllUsers() {
        return ResponseEntity.ok(userRepository.findAll());
    }
}
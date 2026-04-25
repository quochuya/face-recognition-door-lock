package com.huy.smartlock_server;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Optional;

@RestController
@RequestMapping("/api")
public class LockController {

    @Autowired
    private UserRepository userRepository;

    // --- 1. API DÀNH CHO C++ CAMERA ---
    @PostMapping("/detect")
    public ResponseEntity<String> handleDetection(@RequestBody DetectionRequest request) {
        System.out.println("\n[SYSTEM] Tin hieu tu " + request.getCam_id() + " | ID: " + request.getUser_id());

        // Kiểm tra độ nét của khuôn mặt
        if (request.getSai_so() > 65.0) {
            System.out.println("Error: High different (" + request.getSai_so() + ")");
            return ResponseEntity.status(401).body("denied.");
        }

        // Kiểm tra trong Database
        Optional<User> userOpt = userRepository.findById(request.getUser_id());
        if (userOpt.isPresent()) {
            User user = userOpt.get();
            System.out.println("Open: welcome " + user.getFullName() + " - " + user.getRole());
            // TODO: Bắn lệnh sang ESP32 (Module 3) tại đây
            return ResponseEntity.ok("The door is open!");
        } else {
            System.out.println("Stranger " + request.getUser_id() + " in DB.");
            return ResponseEntity.status(403).body("Access denied: user not found.");
        }
    }

    // --- 2. API DÀNH CHO GIAO DIỆN WEB (CRUD) ---
    
    // Lấy danh sách toàn bộ người dùng
    @GetMapping("/users")
    public Iterable<User> getAllUsers() {
        return userRepository.findAll();
    }

    // Thêm người dùng mới
    @PostMapping("/users")
    public User addUser(@RequestBody User newUser) {
        return userRepository.save(newUser);
    }

    // Xóa người dùng theo ID
    @DeleteMapping("/users/{id}")
    public ResponseEntity<String> deleteUser(@PathVariable Integer id) {
        userRepository.deleteById(id);
        return ResponseEntity.ok("Đã xóa!");
    }
}
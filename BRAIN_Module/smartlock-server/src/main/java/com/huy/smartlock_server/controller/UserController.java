package com.huy.smartlock_server.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.dao.DataIntegrityViolationException;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

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


    @PostMapping("/users")
    public ResponseEntity<?> saveUser(@RequestBody User user) {
        try {
            // Lưu bản ghi vào MySQL
            User savedUser = userRepository.save(user);
            return ResponseEntity.ok(savedUser);
        } catch (Exception e) {
            System.err.println("[DB ERROR] Loi khi luu User: " + e.getMessage());
            return ResponseEntity.status(500)
                    .body("{\"status\":\"error\", \"message\":\"Lỗi lưu DB: " + e.getMessage() + "\"}");
        }
    }


    @DeleteMapping("/users/{id}")
    public ResponseEntity<?> deleteUser(@PathVariable Integer id) {
        try {
            User user = userRepository.findById(id).orElse(null);
            
            if (user == null) {
                return ResponseEntity.status(404).body("{\"status\":\"error\", \"message\":\"Khong tim thay nguoi dung\"}");
            }

            if ("ADMIN".equalsIgnoreCase(user.getRole())) {
                System.out.println("[SECURITY ALERT] Co hanh dong co the xoa tai khoan ADMIN (ID: " + id + ")");
                return ResponseEntity.status(403).body("{\"status\":\"error\", \"message\":\"Loi bao mat: Khong duoc phep xoa tai khoan Quan tri vien (ADMIN)\"}");
            }

            userRepository.deleteById(id);

            System.out.println("[SYSTEM] Da xoa User ID: " + id);
            return ResponseEntity.ok().body("{\"status\":\"success\", \"message\":\"Xoa thanh cong\"}");

        } catch (Exception e) {
            System.err.println("[DB ERROR] Loi khi xoa User: " + e.getMessage());
            return ResponseEntity.status(500).body("{\"status\":\"error\", \"message\":\"Lỗi Server: " + e.getMessage() + "\"}");
        }
    }
}
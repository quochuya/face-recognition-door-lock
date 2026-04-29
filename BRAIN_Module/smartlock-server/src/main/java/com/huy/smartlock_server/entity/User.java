package com.huy.smartlock_server.entity;

import jakarta.persistence.*;
import lombok.Data;
import com.fasterxml.jackson.annotation.JsonProperty;

@Entity
@Table(name = "users")
@Data
public class User {
    @Id
    // Giữ nguyên: Không dùng @GeneratedValue để tự đồng bộ ID với OpenCV
    private Integer id; 

    @Column(name = "full_name") // Ép Spring Boot phải tìm đúng cột 'full_name' trong MySQL
    @JsonProperty("fullName")   // Ép Spring Boot khi gửi JSON ra Web phải dùng chữ 'fullName'
    private String fullName;

    @JsonProperty("role")
    private String role; // ADMIN hoặc USER

    @JsonProperty("username")
    private String username;

    @JsonProperty("password")
    private String password;
}
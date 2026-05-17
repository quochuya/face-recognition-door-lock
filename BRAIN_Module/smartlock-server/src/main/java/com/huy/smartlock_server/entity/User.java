package com.huy.smartlock_server.entity;

import jakarta.persistence.*;
import lombok.Data;
import com.fasterxml.jackson.annotation.JsonProperty;
import org.hibernate.annotations.SQLDelete;
import org.hibernate.annotations.Where;

@Entity
@Table(name = "users")
@SQLDelete(sql = "UPDATE users SET is_deleted = 1 WHERE id=?")
@Where(clause = "is_deleted = 0")
@Data
public class User {
    
    @Id
    private Integer id; 

    @Column(name = "full_name") 
    @JsonProperty("fullName")  
    private String fullName;

    @JsonProperty("role")
    private String role; // ADMIN hoặc USER

    // --- CHO PHÉP NULL ĐỂ CÁC USER THƯỜNG KHÔNG CẦN ĐĂNG NHẬP ---
    @Column(name = "username", nullable = true) 
    @JsonProperty("username")
    private String username;

    @Column(name = "password", nullable = true)
    @JsonProperty("password")
    private String password;

    @Column(name = "is_deleted")
    private int isDeleted = 0;
}
package com.huy.smartlock_server.entity;

import jakarta.persistence.*;
import lombok.Data;

@Entity
@Table(name = "users")
@Data
public class User {
    @Id
    private Integer id; // ID này khớp với ID học trong C++

    private String fullName;
    private String role; // ADMIN hoặc USER
    
    private String username; 
    private String password;
}
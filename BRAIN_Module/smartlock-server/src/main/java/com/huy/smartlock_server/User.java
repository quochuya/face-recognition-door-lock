package com.huy.smartlock_server;

import jakarta.persistence.Entity;
import jakarta.persistence.Id;
import jakarta.persistence.Table;

@Entity
@Table(name = "users")
public class User {
    
    @Id
    private Integer id; // ID này sẽ khớp với ID nhận diện bên C++
    private String fullName;
    private String role; // Vị trí (Admin, Khách, Nhân viên)

    public User() {}

    public User(Integer id, String fullName, String role) {
        this.id = id;
        this.fullName = fullName;
        this.role = role;
    }

    // Getters & Setters
    public Integer getId() { return id; }
    public void setId(Integer id) { this.id = id; }
    public String getFullName() { return fullName; }
    public void setFullName(String fullName) { this.fullName = fullName; }
    public String getRole() { return role; }
    public void setRole(String role) { this.role = role; }
}
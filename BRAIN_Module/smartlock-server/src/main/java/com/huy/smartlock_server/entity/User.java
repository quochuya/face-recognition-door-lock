package com.huy.smartlock_server.entity;

import jakarta.persistence.*;
import lombok.Data;
import com.fasterxml.jackson.annotation.JsonProperty;

@Entity
@Table(name = "users")
@Data
public class User {
    @Id
    private Integer id; 

    @Column(name = "full_name") 
    @JsonProperty("fullName")  
    private String fullName;

    @JsonProperty("role")
    private String role; // ADMIN hoặc USER

    @JsonProperty("username")
    private String username;

    @JsonProperty("password")
    private String password;
}
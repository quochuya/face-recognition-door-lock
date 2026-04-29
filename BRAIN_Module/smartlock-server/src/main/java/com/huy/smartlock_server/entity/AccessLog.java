package com.huy.smartlock_server.entity;

import jakarta.persistence.*;
import java.time.LocalDateTime;
import org.hibernate.annotations.CreationTimestamp;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Data;
import lombok.NoArgsConstructor;

@Entity
@Table(name = "access_logs")
@Data // Tự động sinh toàn bộ Getters/Setters
@NoArgsConstructor // Tự động sinh Constructor rỗng bắt buộc của JPA
public class AccessLog {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(name = "user_id")
    private Integer userId;

    @Column(name = "user_name")
    private String userName;

    @Column(name = "camera_name")
    private String cameraName;

    private Double confidence;

    @Column(nullable = false)
    private String status; // "ALLOWED" hoặc "DENIED"

    @CreationTimestamp 
    @JsonProperty("accessTime") 
    private LocalDateTime timestamp;
}
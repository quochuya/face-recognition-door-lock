package com.huy.smartlock_server.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.huy.smartlock_server.entity.Admin;

@Repository
public interface AdminRepository extends JpaRepository<Admin, Long> {
    
}
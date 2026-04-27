package com.huy.smartlock_server.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.huy.smartlock_server.entity.AccessLog;

@Repository
public interface AccessLogRepository extends JpaRepository<AccessLog, Long> {
}
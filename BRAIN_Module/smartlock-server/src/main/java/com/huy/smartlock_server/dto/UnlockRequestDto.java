package com.huy.smartlock_server.dto;

import com.fasterxml.jackson.annotation.JsonProperty;

public class UnlockRequestDto {
    @JsonProperty("cam_id")
    private String cameraName;

    @JsonProperty("user_id")
    private int userId;

    @JsonProperty("sai_so")
    private double distance;

    // Getters
    public String getCameraName() { return cameraName; }
    public int getUserId() { return userId; }
    public double getDistance() { return distance; }

    // Setters
    public void setCameraName(String cameraName) { this.cameraName = cameraName; }
    public void setUserId(int userId) { this.userId = userId; }
    public void setDistance(double distance) { this.distance = distance; }
}
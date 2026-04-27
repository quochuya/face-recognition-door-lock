package com.huy.smartlock_server.dto;

public class UnlockRequestDto {
    private String cameraName;
    private int userId;
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
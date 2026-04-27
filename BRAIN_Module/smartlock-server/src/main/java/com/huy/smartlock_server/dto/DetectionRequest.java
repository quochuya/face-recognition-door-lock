package com.huy.smartlock_server.dto;

public class DetectionRequest {
    private String cam_id;
    private int user_id;
    private double sai_so;

    public String getCam_id() { return cam_id; }
    public void setCam_id(String cam_id) { this.cam_id = cam_id; }
    public int getUser_id() { return user_id; }
    public void setUser_id(int user_id) { this.user_id = user_id; }
    public double getSai_so() { return sai_so; }
    public void setSai_so(double sai_so) { this.sai_so = sai_so; }
}
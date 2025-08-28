package com.foranx.vodchyts.web.dto;

public class StatusUpdateRequest {
    private String status;

    public StatusUpdateRequest(String status) {
        this.status = status;
    }

    public StatusUpdateRequest() {

    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }
}

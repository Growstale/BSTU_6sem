package com.foranx.vodchyts.web.dto;

import java.math.BigDecimal;

public class TransferRequest {
    private String from;
    private String to;
    private BigDecimal amount;

    public TransferRequest() {
    }

    public String getFrom() {
        return from;
    }

    public void setFrom(String from) {
        this.from = from;
    }

    public String getTo() {
        return to;
    }

    public void setTo(String to) {
        this.to = to;
    }

    public BigDecimal getAmount() {
        return amount;
    }

    public void setAmount(BigDecimal amount) {
        this.amount = amount;
    }
}

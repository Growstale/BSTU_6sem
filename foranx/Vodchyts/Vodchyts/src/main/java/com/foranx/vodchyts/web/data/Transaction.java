package com.foranx.vodchyts.web.data;

import java.math.BigDecimal;
import java.time.LocalDateTime;

public class Transaction {
    private String from;
    private String to;
    private BigDecimal amount;
    private String currency;

    public Transaction(String from, String to, BigDecimal amount, String currency) {
        this.from = from;
        this.to = to;
        this.amount = amount;
        this.currency = currency;
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

    public String getCurrency() {
        return currency;
    }

    public void setCurrency(String currency) {
        this.currency = currency;
    }
}

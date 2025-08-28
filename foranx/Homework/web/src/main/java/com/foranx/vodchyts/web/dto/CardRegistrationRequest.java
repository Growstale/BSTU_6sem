package com.foranx.vodchyts.web.dto;

import java.math.BigDecimal;
import java.time.LocalDate;

public class CardRegistrationRequest {
    private String id;
    private String cardNumber;
    private String owner;
    private BigDecimal balance;
    private String currency;
    private LocalDate expiryDate;

    public CardRegistrationRequest(String id, String cardNumber, String owner, BigDecimal balance, String currency, LocalDate expiryDate) {
        this.id = id;
        this.cardNumber = cardNumber;
        this.owner = owner;
        this.balance = balance;
        this.currency = currency;
        this.expiryDate = expiryDate;
    }

    public CardRegistrationRequest() {

    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getCardNumber() {
        return cardNumber;
    }

    public void setCardNumber(String cardNumber) {
        this.cardNumber = cardNumber;
    }

    public String getOwner() {
        return owner;
    }

    public void setOwner(String owner) {
        this.owner = owner;
    }

    public BigDecimal getBalance() {
        return balance;
    }

    public void setBalance(BigDecimal balance) {
        this.balance = balance;
    }

    public String getCurrency() {
        return currency;
    }

    public void setCurrency(String currency) {
        this.currency = currency;
    }

    public LocalDate getExpiryDate() {
        return expiryDate;
    }

    public void setExpiryDate(LocalDate expiryDate) {
        this.expiryDate = expiryDate;
    }
}

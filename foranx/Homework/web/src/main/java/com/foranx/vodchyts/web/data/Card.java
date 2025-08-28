package com.foranx.vodchyts.web.data;

import java.math.BigDecimal;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

public class Card {
    private String id;
    private String cardNumber;
    private String owner;
    private BigDecimal balance;
    private String currency;
    private CardStatus status;
    private LocalDate expiryDate;
    private List<Transaction> transactions = new ArrayList<>();

    public Card(String cardId, String cardNumber, String owner, BigDecimal balance, String currency, LocalDate expiryDate) {
        this.id = cardId;
        this.cardNumber = cardNumber;
        this.owner = owner;
        this.balance = balance;
        this.currency = currency;
        this.status = CardStatus.ACTIVE;
        this.expiryDate = expiryDate;
    }

    public String getId() {
        return id;
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

    public CardStatus getStatus() {
        return status;
    }

    public void setStatus(CardStatus status) {
        this.status = status;
    }

    public LocalDate getExpiryDate() {
        return expiryDate;
    }

    public void setExpiryDate(LocalDate expiryDate) {
        this.expiryDate = expiryDate;
    }

    public List<Transaction> getTransactions() {
        return transactions;
    }

    public void setTransactions(List<Transaction> transactions) {
        this.transactions = transactions;
    }
}


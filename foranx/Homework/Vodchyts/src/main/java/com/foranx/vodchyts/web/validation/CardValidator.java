package com.foranx.vodchyts.web.validation;

import com.foranx.vodchyts.web.data.Card;
import com.foranx.vodchyts.web.data.CardStatus;
import com.foranx.vodchyts.web.exception.InvalidStatusChangeException;
import com.foranx.vodchyts.web.repository.CardRepository;
import org.springframework.stereotype.Component;

import java.time.LocalDate;
import java.util.Optional;

@Component
public class CardValidator {
    private final CardRepository cardRepository;

    public CardValidator(CardRepository cardRepository) {
        this.cardRepository = cardRepository;
    }

    public void validateForCreation(Card card) {
        if (card.getCardNumber() == null || !card.getCardNumber().matches("\\d{16}")) {
            throw new IllegalArgumentException("Incorrect card number format (XXXXXXXXXXXXXXXXXX is required)");
        }
        Optional<Card> existingCardOpt = cardRepository.findByCardNumber(card.getCardNumber());
        if (existingCardOpt.isPresent() && !existingCardOpt.get().getId().equals(card.getId())) {
            throw new IllegalArgumentException("The card number must be unique");
        }
        if (card.getOwner() == null || card.getOwner().isBlank()) {
            throw new IllegalArgumentException("The owner is required");
        }
        if (card.getBalance() == null) {
            throw new IllegalArgumentException("A balance is required");
        }
        if (card.getBalance().signum() < 0) {
            throw new IllegalArgumentException("The balance cannot be negative");
        }
        if (card.getExpiryDate() == null) {
            throw new IllegalArgumentException("The expiration date is required");
        }
        if (card.getExpiryDate().isBefore(LocalDate.now())) {
            throw new IllegalArgumentException("The card has expired");
        }
        if (card.getCurrency() == null || card.getCurrency().isBlank()) {
            throw new IllegalArgumentException("The currency is required");
        }
        if (card.getStatus() == null) {
            throw new IllegalArgumentException("The card's status is required");
        }
    }

    public void validateCardForOperations(Card card) {
        if (card.getStatus() != CardStatus.ACTIVE) {
            throw new IllegalArgumentException("The card " + card.getCardNumber() + " is not active");
        }
        if (card.getExpiryDate().isBefore(LocalDate.now())) {
            throw new IllegalArgumentException("The card " + card.getCardNumber() + " is expired");
        }
    }

    public void validateStatus(CardStatus currentStatus, CardStatus newStatus) {
        boolean valid = switch (newStatus) {
            case ACTIVE -> currentStatus == CardStatus.LOCKED;
            case LOCKED -> currentStatus == CardStatus.ACTIVE;
            case CLOSED -> currentStatus == CardStatus.ACTIVE || currentStatus == CardStatus.LOCKED;
        };

        if (!valid) {
            throw new InvalidStatusChangeException("The status cannot be changed from " +
                    currentStatus + " to " + newStatus);
        }
    }

}

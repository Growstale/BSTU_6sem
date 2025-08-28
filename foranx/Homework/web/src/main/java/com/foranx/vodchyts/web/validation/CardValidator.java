package com.foranx.vodchyts.web.validation;

import com.foranx.vodchyts.web.data.Card;
import com.foranx.vodchyts.web.data.CardStatus;
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
            throw new IllegalArgumentException("Неверный формат номера карты (нужен XXXX-XXXX)");
        }
        Optional<Card> existingCardOpt = cardRepository.findByCardNumber(card.getCardNumber());
        if (existingCardOpt.isPresent() && !existingCardOpt.get().getId().equals(card.getId())) {
            throw new IllegalArgumentException("Номер карты должен быть уникальным");
        }
        if (card.getOwner() == null || card.getOwner().isBlank()) {
            throw new IllegalArgumentException("Владелец обязателен");
        }
        if (card.getBalance() == null) {
            throw new IllegalArgumentException("Баланс обязателен");
        }
        if (card.getBalance().signum() < 0) {
            throw new IllegalArgumentException("Баланс не может быть отрицательным");
        }
        if (card.getExpiryDate() == null) {
            throw new IllegalArgumentException("Дата истечения срока действия обязательна");
        }
        if (card.getExpiryDate().isBefore(LocalDate.now())) {
            throw new IllegalArgumentException("Срок действия карты истёк");
        }
        if (card.getCurrency() == null || card.getCurrency().isBlank()) {
            throw new IllegalArgumentException("Валюта обязательна");
        }
        if (card.getStatus() == null) {
            throw new IllegalArgumentException("Статус карты обязателен");
        }
    }

    public void validateCardForOperations(Card card) {
        if (card.getStatus() != CardStatus.ACTIVE) {
            throw new IllegalArgumentException("Карта " + card.getCardNumber() + " не активна");
        }
        if (card.getExpiryDate().isBefore(LocalDate.now())) {
            throw new IllegalArgumentException("Карта " + card.getCardNumber() + " просрочена");
        }
    }

    public void validateStatus(CardStatus currentStatus, CardStatus newStatus) {
        boolean valid = switch (newStatus) {
            case ACTIVE -> currentStatus == CardStatus.LOCKED;
            case LOCKED -> currentStatus == CardStatus.ACTIVE;
            case CLOSED -> currentStatus == CardStatus.ACTIVE || currentStatus == CardStatus.LOCKED;
        };

        if (!valid) {
            throw new IllegalStateException(
                    "Невозможно изменить статус с " + currentStatus + " на " + newStatus);
        }
    }

}

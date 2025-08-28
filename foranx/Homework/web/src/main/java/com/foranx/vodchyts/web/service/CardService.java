package com.foranx.vodchyts.web.service;

import com.foranx.vodchyts.web.data.Card;
import com.foranx.vodchyts.web.data.CardStatus;
import com.foranx.vodchyts.web.repository.CardRepository;
import com.foranx.vodchyts.web.validation.CardValidator;
import org.springframework.stereotype.Service;

import java.util.Optional;

@Service
public class CardService {

    private final CardRepository repository;
    private final CardValidator validator;

    public CardService(CardRepository repository, CardValidator validator) {
        this.repository = repository;
        this.validator = validator;
    }

    public Card register(Card card) {
        validator.validateForCreation(card);
        repository.save(card);
        return card;
    }

    public Optional<Card> findById(String id) {
        return repository.findById(id);
    }

    public Card updateCardStatus(String cardId, String newStatusString) {
        Optional<Card> cardOpt = repository.findById(cardId);
        if (cardOpt.isEmpty()) throw new IllegalArgumentException("Карта не найдена");
        Card card = cardOpt.get();

        CardStatus newStatus = CardStatus.fromString(newStatusString);
        validator.validateStatus(card.getStatus(), newStatus);

        card.setStatus(newStatus);
        repository.save(card);
        return card;
    }
}

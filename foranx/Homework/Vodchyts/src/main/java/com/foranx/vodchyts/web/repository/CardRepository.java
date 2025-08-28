package com.foranx.vodchyts.web.repository;

import com.foranx.vodchyts.web.data.Card;
import org.springframework.stereotype.Repository;

import java.util.*;

@Repository
public class CardRepository {

    private final Map<String, Card> cardsById = new HashMap<>();
    private final Map<String, Card> cardsByNumber = new HashMap<>();

    public void save(Card card) {
        cardsById.put(card.getId(), card);
        cardsByNumber.put(card.getCardNumber(), card);
    }

    public Optional<Card> findById(String id) {
        return Optional.ofNullable(cardsById.get(id));
    }

    public Optional<Card> findByCardNumber(String number) {
        return Optional.ofNullable(cardsByNumber.get(number));
    }

    public List<Card> findAll() {
        return new ArrayList<>(cardsById.values());
    }
}

package com.foranx.vodchyts.web.controller;

import com.foranx.vodchyts.web.data.Card;
import com.foranx.vodchyts.web.dto.CardRegistrationRequest;
import com.foranx.vodchyts.web.dto.StatusUpdateRequest;
import com.foranx.vodchyts.web.service.CardService;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;
import java.util.Optional;

@RestController
@RequestMapping("/cards")
public class CardController {

    private final CardService cardService;

    public CardController(CardService cardService) {
        this.cardService = cardService;
    }

    @PostMapping("/register")
    public ResponseEntity<?> registerCard(@RequestBody CardRegistrationRequest request) {
        Card card = new Card(
                request.getId(),
                request.getCardNumber(),
                request.getOwner(),
                request.getBalance(),
                request.getCurrency(),
                request.getExpiryDate()
        );

        Card saved = cardService.register(card);
        return new ResponseEntity<>(saved, HttpStatus.CREATED);
    }

    @GetMapping("/{cardId}/balance")
    public ResponseEntity<?> getBalance(@PathVariable String cardId) {
        Optional<Card> cardOpt = cardService.findById(cardId);
        return cardOpt.isPresent()
                ? ResponseEntity.ok(Map.of("balance", cardOpt.get().getBalance()))
                : ResponseEntity.status(HttpStatus.NOT_FOUND).body("Карта не найдена");
    }

    @PutMapping("/{cardId}/status")
    public ResponseEntity<?> updateStatus(@PathVariable String cardId,
                                          @RequestBody StatusUpdateRequest request) {
        Card updated = cardService.updateCardStatus(cardId, request.getStatus());
        return ResponseEntity.ok("Статус обновлён: " + updated.getStatus());
    }
}


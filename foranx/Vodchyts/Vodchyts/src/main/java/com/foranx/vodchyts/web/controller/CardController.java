package com.foranx.vodchyts.web.controller;

import com.foranx.vodchyts.web.data.Card;
import com.foranx.vodchyts.web.dto.CardRegistrationRequest;
import com.foranx.vodchyts.web.dto.StatusUpdateRequest;
import com.foranx.vodchyts.web.service.CardService;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.math.BigDecimal;
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
        System.out.println(">>> Endpoint register >>>");

        System.out.println("Received registerCard request: " + request.getCardNumber());

        Card card = new Card(
                request.getId(),
                request.getCardNumber(),
                request.getOwner(),
                request.getBalance(),
                request.getCurrency(),
                request.getExpiryDate()
        );

        Card saved = cardService.register(card);
        System.out.println("Card registered successfully: " + saved.getCardNumber());
        System.out.println(">>> Endpoint register >>>");
        return new ResponseEntity<>(saved, HttpStatus.CREATED);
    }

    @GetMapping("/{cardNumber}/balance")
    public ResponseEntity<?> getBalance(@PathVariable String cardNumber) {
        System.out.println(">>> Endpoint balance >>>");
        System.out.println("Checking balance for card: " + cardNumber);
        Optional<BigDecimal> balanceOpt = cardService.getBalance(cardNumber);

        if (balanceOpt.isPresent()) {
            System.out.println("Balance found: " + balanceOpt.get());
            System.out.println(">>> Endpoint balance >>>");
            return ResponseEntity.ok(Map.of("balance", balanceOpt.get()));
        } else {
            System.out.println("Card not found: " + cardNumber);
            System.out.println(">>> Endpoint balance >>>");
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body("The card was not found");
        }
    }

    @PutMapping("/{cardNumber}/status")
    public ResponseEntity<?> updateStatus(@PathVariable String cardNumber,
                                          @RequestBody StatusUpdateRequest request) {
        System.out.println(">>> Endpoint status >>>");
        System.out.println("Updating status for card: " + cardNumber + " to " + request.getStatus());
        Card updated = cardService.updateCardStatus(cardNumber, request.getStatus());
        System.out.println("Status updated successfully: " + updated.getStatus());
        System.out.println(">>> Endpoint status >>>");
        return ResponseEntity.ok("Status updated: " + updated.getStatus());
    }
}


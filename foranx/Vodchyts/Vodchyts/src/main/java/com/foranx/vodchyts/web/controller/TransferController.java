package com.foranx.vodchyts.web.controller;

import com.foranx.vodchyts.web.data.Transaction;
import com.foranx.vodchyts.web.dto.TransferRequest;
import com.foranx.vodchyts.web.service.CardService;
import com.foranx.vodchyts.web.service.TransferService;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.math.BigDecimal;
import java.util.List;
import java.util.Map;
import java.util.Optional;

@RestController
public class TransferController {

    private final TransferService transferService;
    private final CardService cardService;

    public TransferController(TransferService transferService,  CardService cardService) {

        this.transferService = transferService;
        this.cardService = cardService;
    }

    @PostMapping("/transfers")
    public ResponseEntity<?> transfer(@RequestBody TransferRequest request) {
        System.out.println(">>> Endpoint transfers >>>");
        System.out.println("Transfer request from " + request.getFrom() +
                " to " + request.getTo() +
                " amount: " + request.getAmount());
        transferService.transfer(request.getFrom(), request.getTo(), request.getAmount());
        System.out.println("The transfer was completed successfully");

        System.out.println("Checking balance for card: " + request.getFrom());
        Optional<BigDecimal> balanceOptFrom = cardService.getBalance(request.getFrom());

        if (balanceOptFrom.isPresent()) {
            System.out.println("Balance found: " + balanceOptFrom.get());
        } else {
            System.out.println("Card not found: " + request.getFrom());
        }

        System.out.println("Checking balance for card: " + request.getTo());
        Optional<BigDecimal> balanceOptTo = cardService.getBalance(request.getTo());

        if (balanceOptTo.isPresent()) {
            System.out.println("Balance found: " + balanceOptTo.get());
        } else {
            System.out.println("Card not found: " + request.getTo());
        }
        System.out.println(">>> Endpoint transfers >>>");
        return ResponseEntity.ok("The transfer was completed successfully");
    }

    @GetMapping("/{cardNumber}/transactions")
    public ResponseEntity<?> getTransactions(@PathVariable String cardNumber,
                                             @RequestParam(name = "limit", defaultValue = "5") int limit) {
        System.out.println(">>> Endpoint transactions >>>");
        System.out.println("Request to receive the latest " + limit +
                " card transactions: " + cardNumber);
        List<Transaction> transactions = transferService.getLastTransactions(cardNumber, limit);
        System.out.println("Transactions found: " + transactions.size());
        System.out.println(">>> Endpoint transactions >>>");
        return ResponseEntity.ok(transactions);
    }
}
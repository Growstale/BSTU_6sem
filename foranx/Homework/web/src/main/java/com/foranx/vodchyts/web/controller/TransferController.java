package com.foranx.vodchyts.web.controller;

import com.foranx.vodchyts.web.data.Transaction;
import com.foranx.vodchyts.web.dto.TransferRequest;
import com.foranx.vodchyts.web.service.TransferService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
public class TransferController {

    private final TransferService transferService;

    public TransferController(TransferService transferService) {

        this.transferService = transferService;
    }

    @PostMapping("/transfers")
    public ResponseEntity<?> transfer(@RequestBody TransferRequest request) {
        transferService.transfer(request.getFrom(), request.getTo(), request.getAmount());
        return ResponseEntity.ok("Перевод выполнен успешно");
    }

    @GetMapping("cards/{cardId}/transactions")
    public ResponseEntity<?> getTransactions(@PathVariable String cardId,
                                             @RequestParam(name = "limit", defaultValue = "5") int limit) {
        List<Transaction> transactions = transferService.getLastTransactions(cardId, limit);
        return ResponseEntity.ok(transactions);
    }
}


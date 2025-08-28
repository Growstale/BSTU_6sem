package com.foranx.vodchyts.web.service;

import com.foranx.vodchyts.web.data.Card;
import com.foranx.vodchyts.web.data.Transaction;
import com.foranx.vodchyts.web.repository.CardRepository;
import com.foranx.vodchyts.web.validation.CardValidator;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.math.BigDecimal;
import java.util.Comparator;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

@Service
public class TransferService {
    private final CardRepository repository;
    private final CardValidator validator;

    public TransferService(CardRepository repository, CardValidator validator) {
        this.repository = repository;
        this.validator = validator;
    }

    @Transactional
    public void transfer(String fromCardNumber, String toCardNumber, BigDecimal amount) {
        if (amount == null || amount.signum() <= 0) {
            throw new IllegalArgumentException("The amount must be greater than zero");
        }

        Optional<Card> fromOpt = repository.findByCardNumber(fromCardNumber);
        if (fromOpt.isEmpty()) throw new IllegalArgumentException("The sender's card was not found");
        Card from = fromOpt.get();

        Optional<Card> toOpt = repository.findByCardNumber(toCardNumber);
        if (toOpt.isEmpty()) throw new IllegalArgumentException("The recipient's card was not found");
        Card to = toOpt.get();

        validator.validateCardForOperations(from);
        validator.validateCardForOperations(to);

        if (!Objects.equals(from.getCurrency(), to.getCurrency())) throw new IllegalArgumentException("The currency cannot be different");
        if (from.getBalance().compareTo(amount) < 0) throw new IllegalArgumentException("Insufficient funds");

        from.setBalance(from.getBalance().subtract(amount));
        to.setBalance(to.getBalance().add(amount));

        Transaction tx = new Transaction(from.getCardNumber(), to.getCardNumber(), amount, from.getCurrency());

        from.getTransactions().add(tx);
        to.getTransactions().add(tx);

        repository.save(from);
        repository.save(to);
    }

    public List<Transaction> getLastTransactions(String cardNumber, int limit) {
        Optional<Card> cardOpt = repository.findByCardNumber(cardNumber);
        if (cardOpt.isEmpty()) throw new IllegalArgumentException("The card was not found");

        Card card = cardOpt.get();
        List<Transaction> txs = card.getTransactions();

        return txs.stream()
                .limit(limit)
                .toList();
    }
}

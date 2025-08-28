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
            throw new IllegalArgumentException("Сумма должна быть больше нуля");
        }

        Optional<Card> fromOpt = repository.findByCardNumber(fromCardNumber);
        if (fromOpt.isEmpty()) throw new IllegalArgumentException("Карта отправителя не найдена");
        Card from = fromOpt.get();

        Optional<Card> toOpt = repository.findByCardNumber(toCardNumber);
        if (toOpt.isEmpty()) throw new IllegalArgumentException("Карта получателя не найдена");
        Card to = toOpt.get();

        validator.validateCardForOperations(from);
        validator.validateCardForOperations(to);

        if (from.getBalance().compareTo(amount) < 0) throw new IllegalArgumentException("Недостаточно средств");

        from.setBalance(from.getBalance().subtract(amount));
        to.setBalance(to.getBalance().add(amount));

        Transaction tx = new Transaction(from.getCardNumber(), to.getCardNumber(), amount, from.getCurrency());

        from.getTransactions().add(tx);
        to.getTransactions().add(tx);

        repository.save(from);
        repository.save(to);
    }

    public List<Transaction> getLastTransactions(String cardId, int limit) {
        Optional<Card> cardOpt = repository.findById(cardId);
        if (cardOpt.isEmpty()) throw new IllegalArgumentException("Карта не найдена");

        Card card = cardOpt.get();
        List<Transaction> txs = card.getTransactions();

        return txs.stream()
                .sorted(Comparator.comparing(Transaction::getDateTime).reversed())
                .limit(limit)
                .toList();
    }
}

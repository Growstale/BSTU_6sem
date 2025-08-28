package com.foranx.vodchyts.web.data;

import java.util.Arrays;
import java.util.stream.Collectors;

public enum CardStatus {
    ACTIVE, LOCKED, CLOSED;

    public static CardStatus fromString(String text) {
        if (text == null) {
            throw new IllegalArgumentException("Статус не может быть пустым");
        }
        try {
            return CardStatus.valueOf(text.trim().toUpperCase());
        } catch (IllegalArgumentException e) {
            String allowedValues = Arrays.stream(CardStatus.values())
                    .map(Enum::name)
                    .collect(Collectors.joining(", "));
            throw new IllegalArgumentException(
                    "Неверный статус '" + text + "'. Допустимые значения: " + allowedValues);
        }
    }
}

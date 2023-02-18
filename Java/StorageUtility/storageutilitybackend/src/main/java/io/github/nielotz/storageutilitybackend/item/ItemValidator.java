package io.github.nielotz.storageutilitybackend.item;

import io.github.nielotz.storageutilitybackend.exception.InvalidItemNameException;

public class ItemValidator {
    static public final int minItemNameLength = 3;
    static public final int maxItemNameLength = 30;

    static public void ensureValidItemName(String itemName) {
        if (itemName == null)
            throw new InvalidItemNameException("Item name cannot be null.");
        if (itemName.length() < minItemNameLength)
            throw new InvalidItemNameException("Item name has to be longer than " + minItemNameLength + " characters.");
        if (itemName.length() > maxItemNameLength)
            throw new InvalidItemNameException("Item name has to be shorter than " + maxItemNameLength + " characters.");
    }
}

package io.github.nielotz.storageutilitybackend.item;

import io.github.nielotz.storageutilitybackend.exception.FoundException;
import io.github.nielotz.storageutilitybackend.exception.InvalidItemNameException;
import io.github.nielotz.storageutilitybackend.exception.NotFoundException;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Objects;
import java.util.Optional;

@Service
public class ItemService {
    private final ItemRepository itemRepository;

    @Autowired
    public ItemService(ItemRepository itemRepository) {
        this.itemRepository = itemRepository;
    }

    public List<Item> getItems() {
        return itemRepository.findAll();
    }

    public void addNewItem(Item item) {
        ensureItemNotExists(item.getName());
        ItemValidator.ensureValidItemName(item.getName());
        itemRepository.save(item);
    }

    public void deleteItem(Long itemId) {
        ensureItemExists(itemId);
        itemRepository.deleteById(itemId);
    }

    private void ensureItemNotExists(String itemName) {
        final Optional<Item> itemOptional = itemRepository.findItemByName(itemName);
        if (itemOptional.isPresent())
            throw new FoundException("Item with name: \"" + itemName + "\" exists.");
    }

    private void ensureItemExists(Long itemId) {
        final boolean exists = itemRepository.existsById(itemId);
        if (!exists)
            throw new NotFoundException("Item with id: " + itemId + " does not exist.");
    }

    @Transactional
    public void updateItem(Long itemId,
                           String newItemName) {
        Item item = itemRepository.findItemById(itemId)
                .orElseThrow(() -> new NotFoundException(
                        "Item with id: " + itemId + " does not exist."));

        if (Objects.equals(newItemName, item.getName()))
            throw new InvalidItemNameException("New item name is the same as current one.");

        ItemValidator.ensureValidItemName(newItemName);
        item.setName(newItemName);
    }
}
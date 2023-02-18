package io.github.nielotz.storageutilitybackend.item.state;

import io.github.nielotz.storageutilitybackend.exception.NotFoundException;
import io.github.nielotz.storageutilitybackend.item.Item;
import io.github.nielotz.storageutilitybackend.item.ItemRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class StateService {
    private final StateRepository stateRepository;
    private final ItemRepository itemRepository;

    @Autowired
    public StateService(StateRepository stateRepository, ItemRepository itemRepository) {
        this.stateRepository = stateRepository;
        this.itemRepository = itemRepository;
    }

    public List<State> getStatesOfItem(Long itemId) {
        ensureItemExists(itemId);
        return stateRepository.findStatesByItemId(itemId);
    }

    public void addNewState(Long itemId, State state) {
        ensureItemExists(itemId);
        state.setItemId(itemId);
        stateRepository.save(state);
    }

    private void ensureItemExists(Long itemId) {
        final Optional<Item> itemOptional = itemRepository.findItemById(itemId);
        if (itemOptional.isEmpty())
            throw new NotFoundException("Item with id: " + itemId + " does not exist.");
    }
}
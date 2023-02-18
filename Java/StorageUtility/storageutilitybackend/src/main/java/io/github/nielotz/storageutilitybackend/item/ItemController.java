package io.github.nielotz.storageutilitybackend.item;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping(path = "api/v1/items")
public class ItemController {

    private final ItemService itemService;

    @Autowired
    public ItemController(ItemService itemService) {
        this.itemService = itemService;
    }

    @GetMapping
    public List<Item> getItems() {
        return itemService.getItems();
    }

    @PostMapping
    public void addNewItem(@RequestBody Item item) {
        itemService.addNewItem(item);
    }

    @DeleteMapping(path = "/{itemId}")
    public void deleteItem(@PathVariable(value="itemId") Long itemId) {
        itemService.deleteItem(itemId);
    }

    @PutMapping(path = "/{itemId}")
    public void updateItem(@PathVariable(value="itemId") Long itemId,
                           @RequestParam String itemName) {
        itemService.updateItem(itemId, itemName);
    }


}

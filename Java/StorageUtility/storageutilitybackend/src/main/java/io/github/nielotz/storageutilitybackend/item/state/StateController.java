package io.github.nielotz.storageutilitybackend.item.state;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping(path = "api/v1/items/{itemId}/states")
public class StateController {

    private final StateService stateService;

    @Autowired
    public StateController(StateService stateService) {
        this.stateService = stateService;
    }

    @GetMapping
    public List<State> getStates(@PathVariable Long itemId) {
        return stateService.getStatesOfItem(itemId);
    }

    @PostMapping
    public void addNewItem(@PathVariable Long itemId,
                           @RequestBody State state) {
        stateService.addNewState(itemId, state);
    }

}

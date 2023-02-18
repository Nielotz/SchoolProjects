package io.github.nielotz.storageutilitybackend.item;

import io.github.nielotz.storageutilitybackend.item.state.State;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface ItemRepository extends JpaRepository<Item, Long> {

    Optional<Item> findItemByName(String name);

    Optional<Item> findItemById(Long id);
}

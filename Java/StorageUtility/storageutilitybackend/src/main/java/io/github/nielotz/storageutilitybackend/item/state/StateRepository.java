package io.github.nielotz.storageutilitybackend.item.state;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface StateRepository extends JpaRepository<State, Long> {
    List<State> findStatesByItemId(Long itemId);
}

package io.github.nielotz.storageutilitybackend.item;

import jakarta.persistence.*;

import java.util.Objects;

@Entity
@Table
public class Item {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;

    private String name = null;

    public Item(Long id, String name) {
        this.id = id;
        this.name = name;
    }

    public Item(String name) {
        this.name = name;
    }

    public Item() {
    }

    public void setId(Long id) {
        this.id = id;
    }

    public Long getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    @Override
    public boolean equals(java.lang.Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        Item item = (Item) o;
        return Objects.equals(this.id, item.id);
    }

    @Override
    public String toString() {
        return "class Items {\n" +
                "    id: " + id + "\n" +
                "    name: " + name + "\n" +
                "}";
    }
}
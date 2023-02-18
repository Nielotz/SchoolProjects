package io.github.nielotz.storageutilitybackend.item.state;

import jakarta.persistence.*;

import java.util.Date;
import java.util.Objects;

@Entity
@Table
public class State {
    public State(Long id, Date statusDate, Long itemId, Long amount) {
        this.id = id;
        this.statusDate = statusDate;
        this.itemId = itemId;
        this.amount = amount;
    }

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;

    @Temporal(TemporalType.TIMESTAMP)
    private Date statusDate = new Date();

    private Long itemId;

    private Long amount;

    public State() {
    }

    public State(Date statusDate, Long itemId, Long amount) {
        this.statusDate = statusDate;
        this.itemId = itemId;
        this.amount = amount;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        State item = (State) o;
        return Objects.equals(this.id, item.id);
    }

    public void setId(Long id) {
        this.id = id;
    }

    public void setName(String name) {

    }

    public void setStatusDate(Date statusDate) {
        this.statusDate = statusDate;
    }

    public void setItemId(Long itemId) {
        this.itemId = itemId;
    }

    public void setAmount(Long amount) {
        this.amount = amount;
    }

    public Long getId() {
        return id;
    }

    public Date getStatusDate() {
        return statusDate;
    }

    public Long getItemId() {
        return itemId;
    }

    public Long getAmount() {
        return amount;
    }

    @Override
    public String toString() {
        return "State{" +
                "id=" + id +
                ", statusDate=" + statusDate +
                ", itemId=" + itemId +
                ", amount=" + amount +
                '}';
    }
}
#include <iostream>

using namespace std;

class SortedArray
{
    /* Search where to put value using binary search.
     *
     * range: <start_idx, end_idx>
     */
    int find_idx_where_value_fit(const int &value, const int start_idx, const int end_idx) const
    {
        const int mid_idx = (start_idx + end_idx) / 2;

        if (value == numbers_[mid_idx])
            return mid_idx;

        if (start_idx > end_idx)
            return start_idx;

        if (value > numbers_[mid_idx])
            return this->find_idx_where_value_fit(value, mid_idx + 1, end_idx);
        return this->find_idx_where_value_fit(value, start_idx, mid_idx - 1);
    }

    int find_idx_where_value_fit(const int &value)
    {
        return find_idx_where_value_fit(value, 0, this->amount_of_numbers_);
    }

public:
    int amount_of_numbers_ = 0;  // Amount of numbers in the array.
    int *numbers_;

    void print()
    {
        for (int i = 0; i < this->amount_of_numbers_; i++)
            cout << this->numbers_[i] << " ";
        cout << endl;
    }

    void insert(const int *const &sorted_numbers, int &amount_of_sorted_numbers)
    {
        for (size_t i = 0; i < amount_of_sorted_numbers; i++)
            this->insert(sorted_numbers[i]);
    }

    void insert(const int &value_to_insert)
    {
        const int idx_where_to_insert = this->find_idx_where_value_fit(value_to_insert);

        // Shift elements.
        for (int idx = this->amount_of_numbers_ - 1; idx >= idx_where_to_insert; idx--)
            this->numbers_[idx + 1] = this->numbers_[idx];

        this->amount_of_numbers_++;
        this->numbers_[idx_where_to_insert] = value_to_insert;
    }

    ~SortedArray()
    {
        delete[] this->numbers_;
    }

    SortedArray(const int *const sorted_numbers, const int amount_of_sorted_numbers, const int array_size, bool are_numbers_ascending)
    {
        this->numbers_ = new int[array_size];

        if (are_numbers_ascending)
            for (int i = 0; i < amount_of_sorted_numbers; i++)
                this->numbers_[i] = sorted_numbers[i];
        else
            for (int i = amount_of_sorted_numbers - 1; i >= 0; i--)
                this->numbers_[i] = sorted_numbers[(amount_of_sorted_numbers - 1) - i];
        this->amount_of_numbers_ = amount_of_sorted_numbers;
    }

    // Sort array of numbers_, which follow: ascending, descending
    // Examples:
    //      1, 2, 4, 3
    //      3, 4, 2, 1
    //      1, 4, 3, 2
    static SortedArray *sort_ascending_descending(const int *const numbers, const int amount_of_numbers)
    {
        // Find last growing element.
        // TODO: Search using binary search.
        int idx_of_last_ascending = -1;
        if (numbers[0] <= numbers[1])
        {
            idx_of_last_ascending = 0;
            for (; idx_of_last_ascending < amount_of_numbers - 1; idx_of_last_ascending++)
                if (numbers[idx_of_last_ascending] >= numbers[idx_of_last_ascending + 1])
                    break;
        }

        // Middle element will be shared.
        const int *ascending_numbers = numbers;
        int amount_of_ascending_numbers = idx_of_last_ascending + 1;

        const int *descending_numbers = numbers + amount_of_ascending_numbers;
        int amount_of_descending_numbers = amount_of_numbers - amount_of_ascending_numbers;

        SortedArray *sorted;
        if (amount_of_ascending_numbers > amount_of_descending_numbers)
        {
            sorted = new SortedArray(ascending_numbers, amount_of_ascending_numbers,
                                     amount_of_numbers, true);
            sorted->insert(descending_numbers, amount_of_descending_numbers);
        }
        else
        {
            if (amount_of_ascending_numbers != 0)
            {
                // Include shared item into bigger array.
                descending_numbers--;
                amount_of_descending_numbers++;
                amount_of_ascending_numbers--;
            }

            sorted = new SortedArray(descending_numbers, amount_of_descending_numbers,
                                     amount_of_numbers, false);
            sorted->insert(ascending_numbers, amount_of_ascending_numbers);

        }
        return sorted;
    }
};

// 1.
int *sort_ascending_descending(const int *const numbers, const int amount_of_numbers)
{
    SortedArray *sorted_numbers = SortedArray::sort_ascending_descending(numbers, amount_of_numbers);
    int *sorted = new int[amount_of_numbers];
    std::move(sorted_numbers->numbers_, sorted_numbers->numbers_ + sorted_numbers->amount_of_numbers_, sorted);
    delete sorted_numbers;
    return sorted;
}


int main()
{
    // cout << "numbers[4] = {1, 2, 4, 3};" << endl;
    int numbers[4] = {1, 2, 4, 3};
    SortedArray *sorted_numbers = SortedArray::sort_ascending_descending(numbers, 4);
    sorted_numbers->print();
    // cout << endl;
    delete sorted_numbers;

    // cout << "numbers2[4] = {3, 4, 2, 1};" << endl;
    int numbers2[4] = {3, 4, 2, 1};
    SortedArray *sorted_numbers2 = SortedArray::sort_ascending_descending(numbers2, 4);
    sorted_numbers2->print();
    // cout << endl;
    delete sorted_numbers2;

    // cout << "numbers3[4] = {1, 4, 3, 2};" << endl;
    int numbers3[4] = {1, 4, 3, 2};
    SortedArray *sorted_numbers3 = SortedArray::sort_ascending_descending(numbers3, 4);
    sorted_numbers3->print();
    // cout << endl;
    delete sorted_numbers3;

    // cout << "numbers4[5] = {1, 4, 5, 3, 2" << endl;
    int numbers4[5] = {1, 4, 5, 3, 2};
    SortedArray *sorted_numbers4 = SortedArray::sort_ascending_descending(numbers4, 5);
    sorted_numbers4->print();
    // cout << endl;
    delete sorted_numbers4;

    int numbers5[5] = {1, 4, 5, 7, 6};
    SortedArray *sorted_numbers5 = SortedArray::sort_ascending_descending(numbers5, 5);
    sorted_numbers5->print();
    delete sorted_numbers5;

    int numbers6[5] = {1, 4, 5, 5, 4};
    SortedArray *sorted_numbers6 = SortedArray::sort_ascending_descending(numbers6, 5);
    sorted_numbers6->print();
    delete sorted_numbers6;

    return 0;
}

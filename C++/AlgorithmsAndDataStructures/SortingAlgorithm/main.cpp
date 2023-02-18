#include <iostream>
#include <cassert>
#include <utility>
#include <vector>

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

    void print() const
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


void print_array(const int *const numbers, const size_t amount_of_numbers)
{
    for (int i = 0; i < amount_of_numbers; i++)
        cout << numbers[i] << " ";
    cout << endl;
}

// 2. Find n numbers where max_number - min_number is smallest.
// Input: amount of numbers, numbers, n ( >= 2)
void quack_sort(int *numbers, const size_t amount_of_numbers)
{
    const int pivot = numbers[amount_of_numbers / 2];
    int left_number_idx = 0;
    int right_number_idx = int(amount_of_numbers) - 1;

    while (left_number_idx <= right_number_idx)
    {
        // Find number to exchange from the left.
        while (numbers[left_number_idx] < pivot)
            left_number_idx++;

        // Find number to exchange from the right.
        while (numbers[right_number_idx] > pivot)
            right_number_idx--;

        if (left_number_idx <= right_number_idx)
        {
            swap(numbers[left_number_idx], numbers[right_number_idx]);
            left_number_idx++;
            right_number_idx--;
        }
    }
    if (0 < right_number_idx)
        quack_sort(numbers, right_number_idx + 1);
    if (left_number_idx < amount_of_numbers - 1)
        quack_sort(numbers + left_number_idx, amount_of_numbers - left_number_idx);
}

int *find_n_numbers_with_smallest_difference(int *numbers, const size_t amount_of_numbers, const size_t &n)
{
    quack_sort(numbers, amount_of_numbers);

    size_t smallest_diff = abs(numbers[0] - numbers[n - 1]);
    size_t smallest_diff_idx = 0;

    size_t first_element_idx = 1;
    size_t last_element_idx = n;
    while (last_element_idx < amount_of_numbers)
    {
        size_t current_diff = abs(numbers[first_element_idx] - numbers[last_element_idx]);
        if (current_diff < smallest_diff)
        {
            smallest_diff = current_diff;
            smallest_diff_idx = first_element_idx;
        }

        first_element_idx++;
        last_element_idx++;
    }

    return numbers + smallest_diff_idx;
}

class Test
{
    struct QuackTestCaseData
    {
        vector<int> numbers;
        vector<int> sorted_numbers;

        QuackTestCaseData(vector<int> numbers, vector<int> sorted_numbers)
                : numbers(std::move(numbers)), sorted_numbers(std::move(sorted_numbers))
        {}
    };

    vector<QuackTestCaseData> quack_sort_tests_case_data = {
            {{1, 3, 2},             {1, 2, 3}},

            {{1, 2, 3},             {1, 2, 3}},
            {{1, 2, 3, 4},          {1, 2, 3, 4}},
            {{1, 2, 3, 4},          {1, 2, 3, 4}},
            {{1, 2, 3, 4, 5},       {1, 2, 3, 4, 5}},

            {{3, 2, 1},             {1, 2, 3}},
            {{4, 3, 2, 1},          {1, 2, 3, 4}},
            {{1, 2, 3, 4},          {1, 2, 3, 4}},
            {{5, 4, 3, 2, 1},       {1, 2, 3, 4, 5}},

            {{3, 3, 3, 4, 1, 1, 2}, {1, 1, 2, 3, 3, 3, 4}},
    };

    struct FindNElementsTestCaseData
    {
        vector<int> numbers;
        vector<int> answer;
        int n;

        FindNElementsTestCaseData(vector<int> numbers, int n, vector<int> answer)
                : numbers(std::move(numbers)), answer(std::move(answer)), n(n)
        {}
    };

    vector<FindNElementsTestCaseData> find_n_elements_tests_case_data = {
            {{1, 1, 1},          2, {1, 1}},
            {{1, 1, 1, 1},       2, {1, 1}},

            {{1, 2, 1},          2, {1, 1}},
            {{1, 2, 1, 3},       2, {1, 1}},

            {{1, 2, 2},          2, {2, 2}},
            {{1, 2, 2, 2},       2, {2, 2}},

            {{1, 2, 4},          2, {1, 2}},
            {{1, 2, 4, 6},       2, {1, 2}},

            {{6, 1, 4},          2, {4, 6}},
            {{6, 1, 4, 9},       2, {4, 6}},

            {{1, 3, 5, 6, 7, 9}, 3, {5, 6, 7}},
    };

    struct SortAscendingDescendingTestCaseData
    {
        vector<int> numbers;
        vector<int> sorted_numbers;

        SortAscendingDescendingTestCaseData(vector<int> numbers, vector<int> sorted_numbers)
                : numbers(std::move(numbers)), sorted_numbers(std::move(sorted_numbers))
        {}
    };

    vector<SortAscendingDescendingTestCaseData> sort_ascending_descending_tests_case_data = {
            {{1, 2, 4, 3},    {1, 2, 3, 4}},
            {{3, 4, 2, 1},    {1, 2, 3, 4}},
            {{1, 4, 3, 2},    {1, 2, 3, 4}},
            {{1, 4, 5, 3, 2}, {1, 2, 3, 4, 5}},
            {{1, 4, 5, 7, 6}, {1, 4, 5, 6, 7}},
            {{1, 4, 5, 5, 4}, {1, 4, 4, 5, 5}},
    };

public:
    void test_quack_sort()
    {
        for (auto &test_case_data: quack_sort_tests_case_data)
        {
            quack_sort(test_case_data.numbers.data(), test_case_data.numbers.size());
            for (int i = 0; i < test_case_data.numbers.size(); i++)
                if (test_case_data.numbers[i] != test_case_data.sorted_numbers[i])
                {
                    cout << "Test failed: " << endl;
                    cout << "\tExpected: ";
                    print_array(test_case_data.sorted_numbers.data(), test_case_data.sorted_numbers.size());

                    cout << "\tGot: ";
                    print_array(test_case_data.numbers.data(), test_case_data.numbers.size());
                    cout << endl;
                    break;
                }
        }
    }

    void test_find_n_numbers_with_smallest_difference()
    {
        for (auto &test_case_data: find_n_elements_tests_case_data)
        {
            int *answer = find_n_numbers_with_smallest_difference(test_case_data.numbers.data(),
                                                                  test_case_data.numbers.size(),
                                                                  test_case_data.n);
            for (int i = 0; i < test_case_data.n; i++)
                if (test_case_data.answer[i] != answer[i])
                {
                    cout << "Test ";
                    print_array(test_case_data.numbers.data(), test_case_data.numbers.size());
                    cout << " failed: " << endl;
                    cout << "\tExpected: ";
                    print_array(test_case_data.answer.data(), test_case_data.answer.size());

                    cout << "\tGot: ";
                    print_array(answer, test_case_data.n);
                    cout << endl;
                    break;
                }
        }
    }

    void test_sort_ascending_descending()
    {
        for (auto &test_case_data: sort_ascending_descending_tests_case_data)
        {
            int *answer = sort_ascending_descending(test_case_data.numbers.data(),
                                                    int(test_case_data.numbers.size()));
            for (int i = 0; i < test_case_data.numbers.size(); i++)
                if (test_case_data.sorted_numbers[i] != answer[i])
                {
                    cout << "Test ";
                    print_array(test_case_data.numbers.data(), test_case_data.numbers.size());
                    cout << " failed: " << endl;

                    cout << "\tExpected: ";
                    print_array(test_case_data.sorted_numbers.data(), test_case_data.sorted_numbers.size());

                    cout << "\tGot: ";
                    print_array(answer, test_case_data.numbers.size());
                    cout << endl;
                    break;
                }
        }
    }
};

int main()
{
    Test().test_quack_sort();
    Test().test_find_n_numbers_with_smallest_difference();
    Test().test_sort_ascending_descending();

    return 0;
}

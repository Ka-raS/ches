#include "cheslib/array.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace ::cheslib;

TEST_CASE("Array: constructor", "[array]") {
    SECTION("Default constructor") {
        Array<int, 4> values;
        CHECK(values.size() == 0);
        CHECK(values.begin() == values.end());
    }

    SECTION("Empty brace initialization") {
        Array<int, 4> values = {};
        CHECK(values.size() == 0);
        CHECK(values.begin() == values.end());

        for (size_t i = 0; i < 4; ++i) {
            const int value = *(values.begin() + i);
            CHECK(value == 0);
        }
    }
}

TEST_CASE("Array: emplace_back", "[array]") {
    Array<int, 4> values;

    values.emplace_back(10);
    values.emplace_back(20);
    values.emplace_back(30);

    CHECK(values.size() == 3);
    CHECK(values[0] == 10);
    CHECK(values[1] == 20);
    CHECK(values[2] == 30);
}

TEST_CASE("Array: pop_back", "[array]") {
    Array<int, 4> values;

    values.emplace_back(1);
    values.emplace_back(2);
    values.emplace_back(3);

    CHECK(values.back() == 3);
    values.pop_back();
    CHECK(values.back() == 2);
    values.pop_back();
    CHECK(values.size() == 1);
    CHECK(values[0] == 1);
}

TEST_CASE("Array: iteration", "[array]") {
    const int samples[4] = {4, 5, 6, 7};

    Array<int, 4> values;
    for (int sample : samples) {
        values.emplace_back(sample);
    }

    size_t index = 0;
    for (int value : values) {
        CHECK(value == samples[index]);
        ++index;
    }

    CHECK(index == 4);
}
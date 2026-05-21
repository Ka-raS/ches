#include <catch2/catch_test_macros.hpp>

#include "cheslib/array.hpp"

using namespace ::cheslib;

TEST_CASE("Array: constructor", "[array]") {
    SECTION("Default initialization") {
        Array<int, 4> values;
        CHECK(values.size() == 0);
        CHECK(values.begin() == values.end());
    }

    SECTION("Empty brace initialization") {
        Array<int, 4> values{};
        values.resize(4);
        CHECK(values.size() == 4);
        CHECK(values.begin() + 4 == values.end());

        for (int value : values) {
            CHECK(value == 0);
        }
    }
}

TEST_CASE("Array: push", "[array]") {
    Array<int, 4> values;

    values.push(10);
    values.push(20);
    values.push(30);

    CHECK(values.size() == 3);
    CHECK(values[0] == 10);
    CHECK(values[1] == 20);
    CHECK(values[2] == 30);
}

TEST_CASE("Array: pop", "[array]") {
    Array<int, 4> values;

    values.push(1);
    values.push(2);
    values.push(3);

    CHECK(values.pop() == 3);
    CHECK(values.pop() == 2);
    CHECK(values.size() == 1);
    CHECK(values[0] == 1);
}

TEST_CASE("Array: iteration", "[array]") {
    int samples[4] = {4, 5, 6, 7};

    Array<int, 4> values;
    for (int sample : samples) {
        values.push(sample);
    }

    size_t index = 0;
    for (int value : values) {
        CHECK(value == samples[index]);
        ++index;
    }

    CHECK(index == 4);
}
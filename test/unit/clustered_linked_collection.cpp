//
// Created by gaelr on 12/30/2024.
//

#include <iostream>
#include <random>
#include <catch2/catch_all.hpp>
#include <glm/detail/type_vec2.hpp>
#include <neon/util/ClusteredLinkedCollection.h>

TEST_CASE("Clustered linked collection init", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;
    REQUIRE(collection.empty());
    REQUIRE(collection.size() == 0);
    REQUIRE(collection.capacity() == 10);
}

TEST_CASE("Clustered linked collection add", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;
    collection.push(10);
    REQUIRE(collection.size() == 1);
    collection.push(20);
    REQUIRE(collection.size() == 2);
    collection.emplace(30);
    REQUIRE(collection.size() == 3);
}

TEST_CASE("Clustered linked collection find and erase", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;
    collection.push(10);
    collection.push(20);
    collection.emplace(30);
    REQUIRE(collection.size() == 3);

    auto it = std::find_if(collection.begin(), collection.end(), [](auto& i) { return i == 20; });
    REQUIRE(it != collection.end());

    collection.erase(it);
    REQUIRE(collection.size() == 2);

    auto it2 = std::find_if(collection.begin(), collection.end(), [](auto& i) { return i == 25; });
    REQUIRE(it2 == collection.end());
    collection.erase(it2);
    REQUIRE(collection.size() == 2);

    for (int value : collection) {
        REQUIRE((value == 10 || value == 30));
    }
}

TEST_CASE("Clustered linked collection find and erase 2", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;

    for (size_t i = 0; i < 100; i++) {
        collection.push(i);
    }

    REQUIRE(collection.size() == 100);

    auto it = std::find_if(collection.begin(), collection.end(), [](auto& i) { return i == 35; });

    REQUIRE(it != collection.end());

    collection.erase(it);
    REQUIRE(collection.size() == 99);

    auto it2 = std::find_if(collection.begin(), collection.end(), [](auto& i) { return i == 100; });
    REQUIRE(it2 == collection.end());
    collection.erase(it2);
    REQUIRE(collection.size() == 99);

    size_t i = 0;
    for (int value : collection) {
        REQUIRE(value != 35);
        ++i;
    }

    REQUIRE(i == 99);
}

TEST_CASE("Clustered linked collection find and erase 3", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;
    std::vector<int> vector;

    vector.reserve(100);
    for (size_t i = 0; i < 100; i++) {
        collection.push(i);
        vector.push_back(i);
    }

    std::ranges::shuffle(vector, std::mt19937());

    for (size_t i = 0; i < 99; i++) {
        auto v1 = vector[i];
        auto v2 = vector[i + 2];
        auto it1 = std::find_if(collection.begin(), collection.end(), [v1](auto& i) { return i == v1; });
        auto it2 = std::find_if(collection.begin(), collection.end(), [v2](auto& i) { return i == v2; });
        collection.erase(it1);
        collection.erase(it2);
        collection.push(v2);
        collection.push(v1);
    }

    REQUIRE(collection.size() == 100);

    for (int value : collection) {
        erase_if(vector, [value](auto& i) { return i == value; });
    }

    REQUIRE(vector.empty());
}

TEST_CASE("Clustered linked collection find and erase 4", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;
    std::vector<int*> vector;

    vector.reserve(100);
    for (size_t i = 0; i < 100; i++) {
        vector.push_back(collection.push(i));
    }

    std::ranges::shuffle(vector, std::mt19937());

    size_t i = vector.size();
    for (auto ptr : vector) {
        REQUIRE(collection.erase(ptr));
        REQUIRE(collection.size() == --i);
    }
}

TEST_CASE("Clustered linked collection clear", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;

    for (size_t i = 0; i < 100; i++) {
        collection.push(i);
    }

    REQUIRE(collection.size() == 100);

    collection.clear();
    REQUIRE(collection.size() == 0);
    REQUIRE(collection.empty());
}

TEST_CASE("Clustered linked collection copy", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;

    for (size_t i = 0; i < 100; i++) {
        collection.push(i);
    }

    auto copy = collection;
    REQUIRE(copy.size() == 100);
    REQUIRE(collection.size() == 100);
}

TEST_CASE("Clustered linked collection move", "[clustered_linked_collection]")
{
    neon::ClusteredLinkedCollection<int, 10> collection;

    for (size_t i = 0; i < 100; i++) {
        collection.push(i);
    }

    auto copy = std::move(collection);
    REQUIRE(copy.size() == 100);
    REQUIRE(collection.empty());
}

TEST_CASE("Clustered linked collection move 2", "[clustered_linked_collection]")
{
    class NoCopyable
    {
      public:
        NoCopyable(const NoCopyable&) = delete;

        NoCopyable() = default;
    };

    neon::ClusteredLinkedCollection<NoCopyable, 10> collection;

    for (size_t i = 0; i < 100; i++) {
        collection.emplace();
    }

    REQUIRE(collection.size() == 100);

    auto copy = std::move(collection);
    REQUIRE(copy.size() == 100);
    REQUIRE(collection.empty());
}

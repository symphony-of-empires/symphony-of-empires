// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      triangle.hpp
//
// Abstract:
//      Small program to test that archive serializer and deserializer works.
// ----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

#include "eng3d/state.hpp"
#include "eng3d/string.hpp"
#include "eng3d/event.hpp"
#include "eng3d/serializer.hpp"

struct GameState : Eng3D::State {
    GameState(const std::vector<std::string>& pkg_paths)
        : Eng3D::State::State(pkg_paths)
    {

    }
    ~GameState() = default;
};

template<typename T>
static int test_numerical_vector(size_t max_samples = 65536) {
    std::vector<T> fuzz(max_samples);
    std::iota(fuzz.begin(), fuzz.end(), 1);
    const auto expected = fuzz;
    {
        Eng3D::Deser::Archive ar;
        Eng3D::Deser::serialize(ar, fuzz);
        ar.to_file("fuzz.bin");
    }

    {
        Eng3D::Deser::Archive ar;
        ar.from_file("fuzz.bin");
        Eng3D::Deser::deserialize(ar, fuzz);

        std::cout << "Elements stored " << fuzz.size() << " expected " << expected.size() << std::endl;
        for(size_t i = 0; i < fuzz.size(); i++) {
            if(fuzz[i] != expected[i]) {
                std::cout << "Test failed, element " << i << " is different (" << fuzz[i] << " != " << expected[i] << ")" << std::endl;
                return -1;
            }
        }
    }
    std::cout << "Test passed" << std::endl;
    return 0;
}

template<typename T, int max_samples = 32>
static int test_numerical_array() {
    std::array<T, max_samples> fuzz;
    std::iota(fuzz.begin(), fuzz.end(), 1);
    const auto expected = fuzz;
    {
        Eng3D::Deser::Archive ar;
        Eng3D::Deser::serialize(ar, fuzz);
        ar.to_file("fuzz.bin");
    }

    {
        Eng3D::Deser::Archive ar;
        ar.from_file("fuzz.bin");
        Eng3D::Deser::deserialize(ar, fuzz);

        std::cout << "Elements stored " << fuzz.size() << " expected " << expected.size() << std::endl;
        for(size_t i = 0; i < fuzz.size(); i++) {
            if(fuzz[i] != expected[i]) {
                std::cout << "Test failed, element " << i << " is different (" << fuzz[i] << " != " << expected[i] << ")" << std::endl;
                return -1;
            }
        }
    }
    std::cout << "Test passed" << std::endl;
    return 0;
}

static int test_string(size_t max_samples = 32) {
    std::string fuzz;
    fuzz.resize(max_samples);

    fuzz[max_samples] = '\0';
    while(--max_samples)
        fuzz[max_samples] = 'A' + (rand() % 26);

    const auto expected = fuzz;
    {
        Eng3D::Deser::Archive ar;
        Eng3D::Deser::serialize(ar, fuzz);
        ar.to_file("fuzz.bin");
    }

    {
        Eng3D::Deser::Archive ar;
        ar.from_file("fuzz.bin");
        Eng3D::Deser::deserialize(ar, fuzz);

        std::cout << "Elements stored " << fuzz.size() << " expected " << expected.size() << std::endl;
        for(size_t i = 0; i < fuzz.size(); i++) {
            if(fuzz[i] != expected[i]) {
                std::cout << "Test failed, element " << i << " is different (" << fuzz[i] << " != " << expected[i] << ")" << std::endl;
                return -1;
            }
        }
    }
    std::cout << "Test passed" << std::endl;
    return 0;
}

int main(int, char**) {
    std::cout << "std::vector" << std::endl;
    test_numerical_vector<int>();
    test_numerical_vector<unsigned int>();
    test_numerical_vector<long long>();
    test_numerical_vector<unsigned long long>();

    std::cout << "std::array" << std::endl;
    test_numerical_array<int>();
    test_numerical_array<unsigned int>();
    test_numerical_array<long long>();
    test_numerical_array<unsigned long long>();

    std::cout << "std::string" << std::endl;
    test_string();
}

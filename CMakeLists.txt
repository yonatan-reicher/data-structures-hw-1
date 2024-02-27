cmake_minimum_required(VERSION 3.23)
project(data_structures_hw1)

set(CMAKE_CXX_STANDARD 14)

add_executable(data_structures_hw1 maina1.cpp Olympicsa1.cpp Olympicsa1.h Country.cpp Country.h
    Team.cpp Team.h tree.h node.h Contestant.cpp Contestant.h StrengthAndId.cpp StrengthAndId.h)

add_executable(team_test_whatsapp ./Tests/team_test_whatsapp.cpp Olympicsa1.cpp Olympicsa1.h Country.cpp Country.h
        Team.cpp Team.h tree.h node.h Contestant.cpp Contestant.h StrengthAndId.cpp StrengthAndId.h)
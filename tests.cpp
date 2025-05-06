#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <iostream>

#include "lib/comp_club.h"

TEST_CASE("CompClub initialization", "[compclub]") {
    SECTION("Valid initialization") {
        REQUIRE_NOTHROW(CompClub(3, "09:00", "23:00", 10));
    }

    SECTION("Invalid initialization") {
        REQUIRE_THROWS(CompClub(0, "09:00", "23:00", 10));
        REQUIRE_THROWS(CompClub(-1, "09:00", "23:00", 10));
        REQUIRE_THROWS(CompClub(3, "09:00", "23:00", 0));
        REQUIRE_THROWS(CompClub(3, "09:00", "23:00", -5));
        REQUIRE_THROWS(CompClub(3, "9:00", "23:00", 10));
        REQUIRE_THROWS(CompClub(3, "09:00", "24:00", 10));
    }
}

TEST_CASE("Client events handling", "[compclub]") {
    CompClub club(3, "09:00", "23:00", 10);

    SECTION("Client arrival") {
        REQUIRE(club.handle_event("09:00 1 client1") == 0);
        REQUIRE(club.handle_event("09:05 1 client1") == 0);
        REQUIRE(club.handle_event("08:59 1 client2") == 1);
    }

    SECTION("Client occupy table") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:00 1 client2");
        REQUIRE(club.handle_event("09:05 2 client1 1") == 0);
        REQUIRE(club.handle_event("09:10 2 client2 1") == 0);
        REQUIRE(club.handle_event("09:15 2 client3 2") == 0);
        REQUIRE_NOTHROW(club.handle_event("09:20 2 client1 4"));
    }

    SECTION("Client waiting") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:00 1 client2");
        club.handle_event("09:00 1 client3");
        club.handle_event("09:00 1 client4");
        club.handle_event("09:05 2 client1 1");
        club.handle_event("09:05 2 client2 2");
        club.handle_event("09:05 2 client3 3");
        REQUIRE(club.handle_event("09:10 3 client4") == 0);
        club.handle_event("09:00 1 client5");
        REQUIRE(club.handle_event("09:15 3 client5") == 0);
        club.handle_event("09:20 4 client1");
        REQUIRE(club.handle_event("09:25 3 client4") == 0);
    }

    SECTION("Client leaving") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:05 2 client1 1");
        REQUIRE(club.handle_event("10:00 4 client1") == 0);
        REQUIRE(club.handle_event("10:05 4 client2") == 0);
    }

    SECTION("Time validation") {
        REQUIRE(club.handle_event("0900 1 client1") == 1);
        REQUIRE(club.handle_event("09-00 1 client1") == 1);
        club.handle_event("10:00 1 client1");
        REQUIRE(club.handle_event("09:59 1 client2") == 1);
    }
}

TEST_CASE("Club closing", "[compclub]") {
    CompClub club(2, "09:00", "12:00", 10);

    club.handle_event("09:00 1 client1");
    club.handle_event("09:00 1 client2");
    club.handle_event("09:00 1 client3");
    club.handle_event("09:05 2 client1 1");
    club.handle_event("09:05 2 client2 2");
    club.handle_event("09:10 3 client3");

    SECTION("Revenue calculation") {
        club.handle_event("10:00 4 client1");
        club.handle_event("11:30 4 client2");
        REQUIRE(club.handle_event("12:00 4 client3") == 0);
    }

    SECTION("Closing with clients inside") {
        REQUIRE_NOTHROW(club.close());
    }
}

TEST_CASE("Edge cases", "[compclub]") {
    SECTION("Single table club") {
        CompClub club(1, "00:00", "23:59", 1);
        club.handle_event("00:00 1 client1");
        club.handle_event("00:00 2 client1 1");
        club.handle_event("00:01 1 client2");
        REQUIRE(club.handle_event("00:02 3 client2") == 0);
        club.handle_event("00:03 1 client3");
        REQUIRE(club.handle_event("00:04 3 client3") == 0);
    }

    SECTION("Midnight crossing") {
        CompClub club(2, "23:00", "01:00", 10);
        club.handle_event("23:00 1 client1");
        club.handle_event("23:00 2 client1 1");
        REQUIRE(club.handle_event("00:30 1 client2") == 0);
        REQUIRE_NOTHROW(club.handle_event("00:59 1 client4"));
        REQUIRE(club.handle_event("01:01 2 client4") == 1);
    }
}

TEST_CASE("Error output checking", "[compclub][output]") {
    CompClub club(2, "09:00", "23:00", 10);

    SECTION("NotOpenYet error") {
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.handle_event("08:59 1 client1");
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "08:59 1 client1\n08:59 13 NotOpenYet\n");
    }

    SECTION("YouShallNotPass error") {
        club.handle_event("09:00 1 client1");
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.handle_event("09:05 1 client1");
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "09:05 1 client1\n09:05 13 YouShallNotPass\n");
    }

    SECTION("PlaceIsBusy error") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:00 1 client2");
        club.handle_event("09:05 2 client1 1");
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.handle_event("09:10 2 client2 1");
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "09:10 2 client2 1\n09:10 13 PlaceIsBusy\n");
    }

    SECTION("ClientUnknown error") {
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.handle_event("09:00 2 client1 1");
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "09:00 2 client1 1\n09:00 13 ClientUnknown\n");
    }

    SECTION("ICanWaitNoLonger error") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:00 1 client3");
        club.handle_event("09:05 2 client1 1");
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.handle_event("09:10 3 client3");
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "09:10 3 client3\n09:10 13 ICanWaitNoLonger!\n");
    }
}

TEST_CASE("Event output checking", "[compclub][output]") {
    CompClub club(2, "09:00", "23:00", 10);

    SECTION("Client occupy from queue output") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:00 1 client2");
        club.handle_event("09:00 1 client3");
        club.handle_event("09:05 2 client1 1");
        club.handle_event("09:05 2 client2 2");
        club.handle_event("09:10 3 client3");
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.handle_event("10:00 4 client1");
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "10:00 4 client1\n10:00 12 client3 1\n");
    }

    SECTION("Club closing output") {
        club.handle_event("09:00 1 client1");
        club.handle_event("09:05 2 client1 1");
        std::stringstream buffer;
        auto old_buf = std::cout.rdbuf(buffer.rdbuf());
        club.close();
        std::cout.rdbuf(old_buf);
        REQUIRE(buffer.str() == "23:00 11 client1\n23:00\n1 140 13:55\n2 0 00:00\n"); // 23 00 - 9 05 -> 14 hours
    }
}
#include <gtest/gtest.h>
#include <limits>
#include "../src/utils/Raffle.h"

namespace Tests
{
    template <typename T>
    std::pair<int, int> GetRange(const std::vector<T>& vec);

    class RaffleTest : public ::testing::Test
    {
    protected:
        RaffleTest()
        {
        }

        ~RaffleTest() = default;

        Util::Raffle<double> realRaffle;
        Util::Raffle<int, std::uniform_int_distribution<int>> intRaffle;
    };

    TEST_F(RaffleTest, NoTickets) 
    {
        // in both cases the 
        EXPECT_EQ(-1, realRaffle.Draw());
        EXPECT_EQ(-1, intRaffle.Draw());
        EXPECT_EQ(0, realRaffle.NumberOfTicketsIssued());
        EXPECT_EQ(0, intRaffle.NumberOfTicketsIssued());
    }

    TEST_F(RaffleTest, BuyZeroTickets)
    {
        const int id = 123;
        realRaffle.BuyTickets(0.0, id);
        intRaffle.BuyTickets(0, id);

        EXPECT_EQ(-1, realRaffle.Draw());
        EXPECT_EQ(-1, intRaffle.Draw());
        EXPECT_EQ(0, realRaffle.NumberOfTicketsIssued());
        EXPECT_EQ(0, intRaffle.NumberOfTicketsIssued());
    }

    TEST_F(RaffleTest, OneParticipant)
    {
        const int id = 123;
        realRaffle.BuyTickets(1.0, id);
        intRaffle.BuyTickets(1, id);

        EXPECT_EQ(id, realRaffle.Draw());
        EXPECT_EQ(id, intRaffle.Draw());
        EXPECT_EQ(1, realRaffle.NumberOfTicketsIssued());
        EXPECT_EQ(1, intRaffle.NumberOfTicketsIssued());
    }

    TEST_F(RaffleTest, TwoParticipants)
    {
        const int id1 = 1;
        const int id2 = 2;
        realRaffle.BuyTickets(0.01, id1);
        realRaffle.BuyTickets(0.01, id2);
        intRaffle.BuyTickets(1, id1);
        intRaffle.BuyTickets(1, id2);

        std::vector<int> realResults;
        std::vector<int> intResults;
        for (int i = 0; i < 50; i++)
        {
            realResults.push_back(realRaffle.Draw());
            intResults.push_back(intRaffle.Draw());
        }

        auto min_max = GetRange(realResults);
        EXPECT_EQ(id1, min_max.first);
        EXPECT_EQ(id2, min_max.second);

        min_max = GetRange(intResults);
        EXPECT_EQ(id1, min_max.first);
        EXPECT_EQ(id2, min_max.second);
    }

    TEST_F(RaffleTest, TestReset)
    {
        const int id1 = 1;
        const int id2 = 2;
        realRaffle.BuyTickets(0.01, id1);
        realRaffle.BuyTickets(0.01, id2);
        intRaffle.BuyTickets(1, id1);
        intRaffle.BuyTickets(1, id2);

        realRaffle.Reset();
        intRaffle.Reset();
        EXPECT_EQ(-1, realRaffle.Draw());
        EXPECT_EQ(-1, intRaffle.Draw());
    }

    template <typename T>
    std::pair<int, int> GetRange(const std::vector<T>& vec)
    {
        std::pair<int, int> result { std::numeric_limits<int>::max(), std::numeric_limits<int>::min() };
        for (auto& v : vec)
        {
            if (v < result.first) result.first = v;
            if (v > result.second) result.second = v;
        }
        return result;
    }
}

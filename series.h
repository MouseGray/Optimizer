#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using Series = std::vector<double>;

Series loadSeries(const std::string& file_name);

enum class Command
{
    Wait,
    Sell,
    Buy
};

template<typename Indicator>
std::tuple<double, int, int> evaluateIndicator(const Series& series,
                                               Indicator functor)
{
    double evaluation = 0.0,
           prev_value = 0.0;
    int    sell_count = 0,
           buy_count  = 0;
    bool   is_position_open = false;

    for(int i = 0; i < series.size(); ++i)
    {
        Command command = functor(i);

        switch(command)
        {
        case Command::Wait:
            break;
        case Command::Sell:
            if(is_position_open)
                evaluation += prev_value - series[i];

            prev_value = series[i];
            ++sell_count;
            is_position_open = true;
            break;
        case Command::Buy:
            if(is_position_open)
                evaluation += series[i] - prev_value;

            prev_value = series[i];
            ++buy_count;
            is_position_open = true;
            break;
        }
    }

    return {evaluation, buy_count, sell_count};
}

template<typename Indicator, std::size_t ...Idx>
Indicator createIndicator(const Series& series,
                          const std::vector<int>& period_it,
                          std::index_sequence<Idx...>)
{
    return Indicator{series, period_it[Idx]...};
}

inline std::string toPeriodText(const std::vector<int>& period_it)
{
    if(period_it.size() == 1)
        return std::to_string(period_it[0]);

    std::string result;

    result += '(';
    for(auto p : period_it)
        result += std::to_string(p) + ", ";
    result.pop_back();
    result.back() = ')';

    return result;
}

template<typename Indicator, int PeriodCount>
std::tuple<double, std::vector<int>> optimizeIndicator(
        const Series& series, const std::vector<std::pair<int, int>>& periods)
{
    std::vector<int> period_it(PeriodCount);

    for(int i = 0; i < PeriodCount; ++i)
        period_it[i] = periods[i].first;

    if(periods.size() < PeriodCount)
        throw std::invalid_argument{"Indicator requires " +
                                    std::to_string(PeriodCount) + " period(s)"};

    auto nextPeriodIt = [&]
    {
        for(int i = PeriodCount - 1; i >= 0; --i)
        {
            ++period_it[i];

            if(period_it[i] < periods[i].second)
                return true;

            period_it[i] = periods[i].first;
        }

        return false;
    };

    double           best_eval   = 0.0;
    std::vector<int> best_period;

    do
    {
        auto indicator = createIndicator<Indicator>(
                    series, period_it,
                    std::make_index_sequence<PeriodCount>{});

        auto [eval, buy, sell] = evaluateIndicator(series, indicator);

        std::cout << "===============\n"
                  << "Period: "     << toPeriodText(period_it) << "\n"
                  << "Evaluation: " << eval   << "\n"
                  << "Buy: "        << buy    << "\n"
                  << "Sell: "       << sell   << "\n";

        if(best_period.empty() || eval > best_eval)
        {
            best_eval = eval;
            best_period = period_it;
        }
    }
    while(nextPeriodIt());

    std::cout << "===============\n";

    return {best_eval, best_period};
}
















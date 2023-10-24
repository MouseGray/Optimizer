#include "sma.h"

#include <iostream>
#include <numeric>
#include <limits>
#include <tuple>

#include "utils.h"

static double getSMAPoint(const Series& series, int n, int period)
{
    double sum = 0.0;

    for(int i = n - period + 1; i < n + 1; ++i)
        sum += series[i];

    return sum / period;
}

Series createSMASeries(const Series& series, int period)
{
    Series SMA_series;

    for(int i = period - 1, size = series.size(); i < size; ++i)
        SMA_series.push_back(getSMAPoint(series, i, period));

    return SMA_series;
}

SMAIndicator::SMAIndicator(const Series& series, int period) :
    series_{series},
    SMA_{createSMASeries(series, period)}
{

}

Command SMAIndicator::operator()(int i) const
{
    int SMA_offset = series_.size() - SMA_.size();

    if(i < SMA_offset + 1)
        return Command::Wait;

    int SMA_i = i - SMA_offset;

    auto relation = getSeriesRelation(
                std::reverse_iterator{SMA_.begin() + SMA_i},
                std::reverse_iterator{SMA_.begin()},
                std::reverse_iterator{series_.begin() + i});

    switch(relation)
    {
    case Relation::Equal:
        return Command::Wait;
    case Relation::Below:
        if(SMA_[SMA_i] > series_[i])
            return Command::Sell;
        break;
    case Relation::Above:
        if(SMA_[SMA_i] < series_[i])
            return Command::Buy;
        break;
    }

    return Command::Wait;
}

SMAPairIndicator::SMAPairIndicator(const Series& series, int fast_period,
                                   int slow_period) :
    series_{series},
    fast_SMA_{createSMASeries(series, fast_period)},
    slow_SMA_{createSMASeries(series, slow_period)}
{

}

Command SMAPairIndicator::operator()(int i) const
{
    int slow_SMA_offset = series_.size() - slow_SMA_.size();
    int fast_SMA_offset = series_.size() - fast_SMA_.size();

    if(i < slow_SMA_offset + 1)
        return Command::Wait;

    int slow_SMA_i = i - slow_SMA_offset;
    int fast_SMA_i = i - fast_SMA_offset;

    auto relation = getSeriesRelation(
                std::reverse_iterator{slow_SMA_.begin() + slow_SMA_i},
                std::reverse_iterator{slow_SMA_.begin()},
                std::reverse_iterator{fast_SMA_.begin() + fast_SMA_i});

    switch(relation)
    {
    case Relation::Equal:
        return Command::Wait;
    case Relation::Below:
        if(fast_SMA_[fast_SMA_i] < slow_SMA_[slow_SMA_i])
            return Command::Sell;
        break;
    case Relation::Above:
        if(fast_SMA_[fast_SMA_i] > slow_SMA_[slow_SMA_i])
            return Command::Buy;
        break;
    }

    return Command::Wait;
}

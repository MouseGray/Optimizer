#include "macd.h"

#include <tuple>

#include "sma.h"
#include "utils.h"

Series createMACDSeries(const Series& series, int fast_period, int slow_period)
{
    auto fast_sma_series = createSMASeries(series, fast_period);
    auto slow_sma_series = createSMASeries(series, slow_period);

    auto fast_i = fast_sma_series.size() - slow_sma_series.size();
    auto slow_i = 0;

    Series MACD_series;

    for(int size = slow_sma_series.size(); slow_i < size; ++slow_i, ++fast_i)
        MACD_series.push_back(slow_sma_series[slow_i] -
                              fast_sma_series[fast_i]);

    return MACD_series;
}

MACDIndicator::MACDIndicator(const Series& series, int fast_period,
                             int slow_period, int signal_line_period) :
    series_{series},
    MACD_{createMACDSeries(series, fast_period, slow_period)},
    signal_line_{createSMASeries(MACD_, signal_line_period)}
{

}

Command MACDIndicator::operator()(int i) const
{
    int signal_line_offset = series_.size() - signal_line_.size();
    int MACD_offset = series_.size() - MACD_.size();

    if(i < signal_line_offset + 1)
        return Command::Wait;

    int signal_line_i = i - signal_line_offset;
    int MACD_i = i - MACD_offset;

    auto relation = getSeriesRelation(
                std::reverse_iterator{signal_line_.begin() + signal_line_i},
                std::reverse_iterator{signal_line_.begin()},
                std::reverse_iterator{MACD_.begin() + MACD_i});

    switch(relation)
    {
    case Relation::Equal:
        return Command::Wait;
    case Relation::Below:
        if(signal_line_[signal_line_i] > MACD_[MACD_i])
            return Command::Sell;
        break;
    case Relation::Above:
        if(signal_line_[signal_line_i] < MACD_[MACD_i])
            return Command::Buy;
        break;
    }

    return Command::Wait;
}

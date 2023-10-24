#pragma once

#include "series.h"

Series createMACDSeries(const Series& series, int fast_period, int slow_period);

class MACDIndicator
{
public:
    MACDIndicator(const Series& series, int fast_period, int slow_period,
                  int signal_line_period);

    Command operator()(int i) const;
private:
    const Series& series_;
    Series        MACD_;
    Series        signal_line_;
};

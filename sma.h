#pragma once

#include "series.h"

Series createSMASeries(const Series& series, int period);

class SMAIndicator
{
public:
    SMAIndicator(const Series& series, int period);

    Command operator()(int i) const;
private:
    const Series& series_;
    Series        SMA_;
};

class SMAPairIndicator
{
public:
    SMAPairIndicator(const Series& series, int fast_period, int slow_period);

    Command operator()(int i) const;
private:
    const Series& series_;
    Series        fast_SMA_;
    Series        slow_SMA_;
};

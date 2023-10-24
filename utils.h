#pragma once

#include "series.h"

enum class Relation
{
    Equal,
    Below,
    Above
};

Relation getSeriesRelation(Series::const_reverse_iterator major_begin,
                           Series::const_reverse_iterator major_end,
                           Series::const_reverse_iterator minor_begin);

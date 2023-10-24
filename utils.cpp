#include "utils.h"

#include <assert.h>

Relation getSeriesRelation(Series::const_reverse_iterator major_begin,
                           Series::const_reverse_iterator major_end,
                           Series::const_reverse_iterator minor_begin)
{
    if(major_begin == major_end)
        return Relation::Equal;

    for(; major_begin != major_end; ++major_begin, ++minor_begin)
    {
        if(*major_begin < *minor_begin)
            return Relation::Below;

        if(*major_begin > *minor_begin)
            return Relation::Above;
    }

    return Relation::Equal;
}

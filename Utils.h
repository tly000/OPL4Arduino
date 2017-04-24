#pragma once

/*
 * Utils.h
 *
 *  Created on: 22.09.2016
 *      Author: tly
 */

template<typename T> inline int log2_fast(T d) {
    int result;
    frexp(d, &result);
    return result-1;
}



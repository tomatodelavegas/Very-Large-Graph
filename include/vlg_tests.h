//
// Created by geoffrey on 28/05/2020.
//

#ifndef VLG_VLG_TESTS_H
#define VLG_VLG_TESTS_H

#include "magnien_prelim.h"
#include "magnien_utils.h"
#include "magnien_distances.h"
#include "vlg_utils.h"
#include "vlg_center.h"

#include <assert.h>

void test_leafs_detection(graph *g, int v);

void test_leafs_rm_lw_than(graph *g, int v);

void test_pop_farthest(graph *g, int v);

#endif //VLG_VLG_TESTS_H
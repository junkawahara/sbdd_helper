/* *************** import functions */

/* Frees the working vectors of bddimportbddasknuth_inner and returns */
/* bddnull from it. Used only there (after all the three vectors are */
/* initialized) and undefined after it. */
#define sbddextended_freeVectorsAndReturnNull() \
    do { \
        sbddextended_MyVector_deinitialize(&hi_vec); \
        sbddextended_MyVector_deinitialize(&lo_vec); \
        sbddextended_MyVector_deinitialize(&level_vec); \
        return bddnull; \
    } while (0)

/* Frees the working data of the node construction loop of */
/* bddimportbddasknuth_inner and returns bddnull from it. */
/* Used only there and undefined after it. */
#define sbddextended_freeNodeBufAndReturnNull() \
    do { \
        for (j = (llint)lo_vec.count - 1; j > i; --j) { \
            bddfree(bddnode_buf[j]); \
        } \
        free(bddnode_buf); \
        sbddextended_freeVectorsAndReturnNull(); \
    } while (0)

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth_inner(FILE* fp, int is_hex, int root_level,
                                int is_zbdd
#ifdef __cplusplus
                                , ReadLineObject& sbddextended_readLine
#endif
                                )
{
    int c, level, level_count = 1;
    llint i, j, id, lo, hi, line_count = 0;
    ullint idu, lou, hiu;
    bddvar var;
    char buf[sbddextended_BUFSIZE];
    bddp p, p0, p1, pf, pfn;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec, lo_vec, hi_vec;

    /* To avoid compiler warning, we initialize it here. */
    sbddextended_MyVector_initialize(&level_vec);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if ((buf[0] == '0' && buf[1] == '\n') ||
                (buf[0] == '0' && buf[1] == '\0')) {
            sbddextended_MyVector_deinitialize(&level_vec);
            return bddgetterminal(0, is_zbdd);
        } else if ((buf[0] == '1' && buf[1] == '\n') ||
                (buf[0] == '1' && buf[1] == '\0')) {
            sbddextended_MyVector_deinitialize(&level_vec);
            return bddgetterminal(1, is_zbdd);
        } else if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                /* lo_vec and hi_vec are not initialized yet. */
                sbddextended_MyVector_deinitialize(&level_vec);
                return bddnull;
            }
            if (level != level_count) {
                fprintf(stderr, "Format error in line %lld: the level "
                        "header must be #%d\n", line_count, level_count);
                /* lo_vec and hi_vec are not initialized yet. */
                sbddextended_MyVector_deinitialize(&level_vec);
                return bddnull;
            }
            ++level_count;
            sbddextended_MyVector_add(&level_vec, (llint)2);
            break;
        }
    }

    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    sbddextended_MyVector_add(&lo_vec, 0ll);
    sbddextended_MyVector_add(&lo_vec, 1ll);
    sbddextended_MyVector_add(&hi_vec, 0ll);
    sbddextended_MyVector_add(&hi_vec, 1ll);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                sbddextended_freeVectorsAndReturnNull();
            }
            if (level != level_count) {
                fprintf(stderr, "Format error in line %lld: the level "
                        "header must be #%d\n", line_count, level_count);
                sbddextended_freeVectorsAndReturnNull();
            }
            ++level_count;
            sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);
        } else {
            if (is_hex) {
                c = sscanf(buf, "%llx:%llx,%llx", &idu, &lou, &hiu);
                id = (llint)idu;
                lo = (llint)lou;
                hi = (llint)hiu;
            } else {
                c = sscanf(buf, "%lld:%lld,%lld", &id, &lo, &hi);
            }
            if (c < 3) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                sbddextended_freeVectorsAndReturnNull();
            }
            if (id != (llint)lo_vec.count) {
                fprintf(stderr, "Format error in line %lld\n",
                        line_count);
                sbddextended_freeVectorsAndReturnNull();
            }
            sbddextended_MyVector_add(&lo_vec, lo);
            sbddextended_MyVector_add(&hi_vec, hi);
        }
    }
    sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);
    /* level_vec holds one entry per level header line and one more added */
    /* just above, and the checks of the header lines guarantee that the */
    /* headers are #1, #2, ..., so the following is an invariant of this */
    /* function rather than a check of the input. */
    assert(level_count == (int)level_vec.count);

    /* The input contains no node (the file is empty or consists only of */
    /* level header lines). Note that the root node must exist at the */
    /* index sbddextended_BDDNODE_START of bddnode_buf. */
    if ((llint)lo_vec.count <= sbddextended_BDDNODE_START) {
        fprintf(stderr, "Unexpected end of the input.\n");
        sbddextended_freeVectorsAndReturnNull();
    }

    if (root_level < 0) {
        root_level = level_count - 1;
    } else if (root_level < level_count - 1) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        sbddextended_freeVectorsAndReturnNull();
    }

    while (bddvarused() < (bddvar)root_level) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc(lo_vec.count * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = bddgetterminal(0, is_zbdd);
    bddnode_buf[1] = bddgetterminal(1, is_zbdd);

    for (i = (llint)lo_vec.count - 1; i >= sbddextended_BDDNODE_START; --i) {
        for (level = 1; level < (llint)level_vec.count; ++level) {
            if (sbddextended_MyVector_get(&level_vec, (llint)level - 1) <= i &&
                i < sbddextended_MyVector_get(&level_vec, (llint)level)) {
                break;
            }
        }
        /* The entries of level_vec are non-decreasing and the last one is */
        /* lo_vec.count, so the loop above always finds the level of i, and */
        /* root_level >= level_count - 1 >= level has been checked, so the */
        /* following are invariants rather than checks of the input. */
        assert(level < (llint)level_vec.count);
        assert((1 <= root_level - level + 1) && ((root_level - level + 1) <= (int)bddvarused()));
        lo = sbddextended_MyVector_get(&lo_vec, i);
        hi = sbddextended_MyVector_get(&hi_vec, i);
        /* A child must be a terminal or a node whose id is larger than */
        /* that of the current node, that is, an already constructed one. */
        if (!(0 <= lo && lo < sbddextended_BDDNODE_START) &&
                !(i < lo && lo < (llint)lo_vec.count)) {
            fprintf(stderr, "The 0-child (%lld) of the node %lld is "
                    "out of range.\n", lo, i);
            sbddextended_freeNodeBufAndReturnNull();
        }
        if (!(0 <= hi && hi < sbddextended_BDDNODE_START) &&
                !(i < hi && hi < (llint)lo_vec.count)) {
            fprintf(stderr, "The 1-child (%lld) of the node %lld is "
                    "out of range.\n", hi, i);
            sbddextended_freeNodeBufAndReturnNull();
        }
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zbdd == 0) { /* BDD */
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand(bddnode_buf[lo], pfn);
            p1 = bddand(bddnode_buf[hi], pf);
            bddnode_buf[i] = bddor(p0, p1);
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { /* ZDD */
            p0 = bddnode_buf[lo];
            p1 = bddchange(bddnode_buf[hi], var);
            bddnode_buf[i] = bddunion(p0, p1);
            bddfree(p1);
        }
    }
    for (i = (llint)lo_vec.count - 1;
            i >= sbddextended_BDDNODE_START + 1; --i) {
        bddfree(bddnode_buf[i]);
    }

    p = bddnode_buf[sbddextended_BDDNODE_START];

    free(bddnode_buf);

    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);

    return p;
}

#undef sbddextended_freeNodeBufAndReturnNull
#undef sbddextended_freeVectorsAndReturnNull

#ifdef __cplusplus

sbddextended_INLINE_FUNC
BDD importBDDAsKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasknuth_inner(fp, (is_hex ? 1 : 0),
                                            root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD importBDDAsKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasknuth_inner(NULL, (is_hex ? 1 : 0),
                                            root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasknuth_inner(fp, (is_hex ? 1 : 0),
                                            root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasknuth_inner(NULL, (is_hex ? 1 : 0),
                                            root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 0, glo);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 1, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth(FILE* fp, int is_hex, int root_level)
{
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 0);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasknuth(FILE* fp, int is_hex, int root_level)
{
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 1);
}

#endif


/* *************** export functions */


sbddextended_INLINE_FUNC
void bddexportbddasknuth_inner(FILE* fp, bddp f, int is_hex,
                                bddNodeIndex* node_index,
                                int is_zbdd
#ifdef __cplusplus
                                    , const WriteObject& sbddextended_writeLine
#endif
                                    )
{
    int clevel, i, is_making_index = 0;
    bddp node, n0, n1;
    llint id0, id1, k;
    char ss[sbddextended_BUFSIZE];

    if (f == bddnull) {
        return;
    }

    if (f == bddempty) {
        sbddextended_writeLine("0", fp);
        return;
    } else if (f == bddsingle) {
        sbddextended_writeLine("1", fp);
        return;
    }

    if (is_zbdd < 0 && !(f == bddempty || f == bddsingle)) {
        if (bddiszbdd(f) != 0) {
            is_zbdd = 1;
        } else {
            is_zbdd = 0;
        }
    }

    if (node_index != NULL) {
        if (node_index->is_raw != 0) {
            fprintf(stderr, "The node index must not be constructed "
                    "in the raw mode.\n");
            return;
        }
    }

    if (node_index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            node_index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            node_index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    for (i = node_index->height; i >= 1; --i) {
        sbddextended_snprintf1(ss, sbddextended_BUFSIZE, "#%d", node_index->height - i + 1);
        sbddextended_writeLine(ss, fp);
        for (k = 0; k < (llint)node_index->level_vec_arr[i].count; ++k) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], k);
            n0 = bddgetchild0g(node, is_zbdd, 0);
            if (n0 == bddempty) {
                id0 = 0;
            } else if (n0 == bddsingle) {
                id0 = 1;
            } else {
                clevel = (int)bddgetlev(n0);
                if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                (llint)n0, &id0) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                id0 += node_index->offset_arr[clevel];
            }
            n1 = bddgetchild1g(node, is_zbdd, 0);
            if (n1 == bddempty) {
                id1 = 0;
            } else if (n1 == bddsingle) {
                id1 = 1;
            } else {
                clevel = (int)bddgetlev(n1);
                if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                (llint)n1, &id1) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                id1 += node_index->offset_arr[clevel];
            }
            if (is_hex) {
                sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                    "%llx:%llx,%llx", node_index->offset_arr[i] + k,
                    id0, id1);
            } else {
                sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                    "%lld:%lld,%lld", node_index->offset_arr[i] + k,
                    id0, id1);
            }
            sbddextended_writeLine(ss, fp);
        }
    }
    if (is_making_index) {
        bddNodeIndex_destruct(node_index);
        free(node_index);
    }
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsKnuth(FILE* fp, const BDD& bdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, bdd.GetID(), (is_hex ? 1 : 0), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsKnuth(FILE* fp, const BDD& bdd, bool is_hex = false)
{
    exportBDDAsKnuth<int>(fp, bdd, is_hex, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsKnuth(std::ostream& ost, const BDD& bdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasknuth_inner(NULL, bdd.GetID(), (is_hex ? 1 : 0), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsKnuth(std::ostream& ost, const BDD& bdd, bool is_hex = false)
{
    exportBDDAsKnuth<int>(ost, bdd, is_hex, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex = false)
{
    exportZBDDAsKnuth<int>(fp, zbdd, is_hex, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasknuth_inner(NULL, zbdd.GetID(), (is_hex ? 1 : 0), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex = false)
{
    exportZBDDAsKnuth<int>(ost, zbdd, is_hex, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, f, (is_hex ? 1 : 0), node_index, 0, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, f, (is_hex ? 1 : 0), node_index, 1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    bddexportbddasknuth_inner(fp, f, is_hex, node_index, 0);
}

sbddextended_INLINE_FUNC
void bddexportzbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    bddexportbddasknuth_inner(fp, f, is_hex, node_index, 1);
}

#endif

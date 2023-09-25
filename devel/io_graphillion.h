
sbddextended_INLINE_FUNC
bddp bddconstructbddfromgraphillion_inner(FILE* fp, int root_level, int is_zdd
#ifdef __cplusplus
                             , ReadLineObject& sbddextended_readLine
#endif
                             )
{
    int c, level = 0;
    llint i, id, lo, hi, line_count = 0;
    llint max_node_id = 0, root_node_id = 0;
    bddvar var;
    char buf[sbddextended_MAX_LINE];
    char buf1[sbddextended_MAX_LINE];
    char buf2[sbddextended_MAX_LINE];
    char buf3[sbddextended_MAX_LINE];
    char buf4[sbddextended_MAX_LINE];
    bddp p, p0, p1, pf, pfn;
    bddp* bddnode_buf;
    sbddextended_MyVector node_vec, level_vec, lo_vec, hi_vec;

    sbddextended_MyVector_initialize(&node_vec);
    sbddextended_MyVector_initialize(&level_vec);
    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '.') { // end of file
            break;
        }
        // # Fix ME!  buffer over run
        c = sscanf(buf, "%s %s %s %s", buf1, buf2, buf3, buf4);
        if (c < 4) {
            fprintf(stderr, "Format error in line %lld\n", line_count);
            exit(1);
        }
        if (buf3[0] == 'B') {
            lo = 0;
        } else if (buf3[0] == 'T') {
            lo = 1;
        } else { // # Fix ME! atoi -> atoll ?
            lo = atoi(buf3) + 2;
        }
        if (buf4[0] == 'B') {
            hi = 0;
        } else if (buf4[0] == 'T') {
            hi = 1;
        } else { // # Fix ME! atoi -> atoll ?
            hi = atoi(buf4) + 2;
        }
        sbddextended_MyVector_add(&node_vec, (llint)atoi(buf1) + 2);
        sbddextended_MyVector_add(&level_vec, (llint)atoi(buf2));
        sbddextended_MyVector_add(&lo_vec, (llint)lo);
        sbddextended_MyVector_add(&hi_vec, (llint)hi);
        if (max_node_id < (llint)atoi(buf1) + 2) {
            max_node_id = (llint)atoi(buf1) + 2;
        }
        if (level < (llint)atoi(buf2)) {
            level = (llint)atoi(buf2);
        }
    }

    if (root_level < 0) {
        root_level = level;
    } else if (root_level < level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        exit(1);
    }

    while ((int)bddvarused() < level) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc((ullint)(max_node_id + 1) * sizeof(bddp));
    bddnode_buf[0] = (is_zdd == 0 ? bddfalse : bddempty);
    bddnode_buf[1] = (is_zdd == 0 ? bddtrue : bddsingle);

    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        level = (int)sbddextended_MyVector_get(&level_vec, i);
        lo = sbddextended_MyVector_get(&lo_vec, i);
        hi = sbddextended_MyVector_get(&hi_vec, i);
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { // BDD
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand(bddnode_buf[lo], pfn);
            p1 = bddand(bddnode_buf[hi], pf);
            bddnode_buf[id] = bddor(p0, p1);
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { // ZDD
            p0 = bddnode_buf[lo];
            p1 = bddchange(bddnode_buf[hi], var);
            bddnode_buf[id] = bddunion(p0, p1);
            bddfree(p1);
        }
        if (level == 1) {
            root_node_id = id;
        }
    }
    // # FIX ME: need bddfree
    //for (i = (llint)lo_vec.count - 1;
    //     i >= sbddextended_BDDNODE_START + 1; --i) {
    //    bddfree(bddnode_buf[i]);
    //}

    p = bddnode_buf[root_node_id];

    free(bddnode_buf);

    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);
    sbddextended_MyVector_deinitialize(&node_vec);

    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromGraphillion(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfromgraphillion_inner(NULL, root_level, 1, glo);
    return ZBDD_ID(p);
}

#endif

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion_inner(FILE* fp, bddp f,
                                   bddNodeIndex* index, int is_zbdd
#ifdef __cplusplus
                        , const WriteObject& sbddextended_writeLine
#endif
                             )
{
    int i, k, n;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    char ss[sbddextended_TEMP_BUFSIZE];

    if (f == bddnull) {
        return;
    }

    if (is_zbdd < 0 && !(f == bddempty || f == bddsingle)) {
        if (bddiszbdd(f) != 0) {
            is_zbdd = 1;
        } else {
            is_zbdd = 0;
        }
    }

    if (index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    if (f == bddempty) {
        sbddextended_writeLine("B", fp);
        sbddextended_writeLine(".", fp);
        return;
    } else if (f == bddsingle) {
        sbddextended_writeLine("T", fp);
        sbddextended_writeLine(".", fp);
        return;
    }

    for (i = 1; i <= index->height; ++i) {
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            n = sprintf(ss, "%lld %d", (llint)node, i);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_zbdd != 0) {
                    child = bddgetchildz(node, k);
                } else {
                    child = bddgetchildb(node, k);
                }
                if (!bddisterminal(child)) {
                    n += sprintf(ss + n, " %lld", (llint)child);
                } else if (child == bddempty) {
                    n += sprintf(ss + n, " B");
                } else if (child == bddsingle) {
                    n += sprintf(ss + n, " T");
                }
            }
            sbddextended_writeLine(ss, fp);
        }
    }

    sbddextended_writeLine(".", fp);

    if (is_making_index) {
        bddNodeIndex_destruct(index);
    }
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(FILE* fp, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphillion_inner(fp, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(std::ostream& ost, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddwritebddforgraphillion_inner(NULL, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion(FILE* fp, bddp f,
                               bddNodeIndex* index)
{
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphillion_inner(fp, f, index, -1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion(FILE* fp, bddp f,
                               bddNodeIndex* index)
{
    bddwritebddforgraphillion_inner(fp, f, index, -1);
}

#endif

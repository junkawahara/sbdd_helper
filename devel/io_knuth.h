// *************** import functions

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth_inner(FILE* fp, int is_hex, int root_level,
                                int is_zdd
#ifdef __cplusplus
                                , ReadLineObject& sbddextended_readLine
#endif
                                )
{
    int c, level, level_count = 1;
    llint i, id, lo, hi, line_count = 0;
    ullint idu, lou, hiu;
    bddvar var;
    char buf[sbddextended_MAX_LINE];
    bddp p, p0, p1, pf, pfn;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec, lo_vec, hi_vec;

    sbddextended_MyVector_initialize(&level_vec);
    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    sbddextended_MyVector_add(&lo_vec, 0);
    sbddextended_MyVector_add(&lo_vec, 1);
    sbddextended_MyVector_add(&hi_vec, 0);
    sbddextended_MyVector_add(&hi_vec, 1);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                exit(1);
            }
            assert(level == level_count);
            ++level_count;
            sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);
            break;
        }
    }

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                exit(1);
            }
            assert(level == level_count);
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
                exit(1);
            }
            if (id != (llint)lo_vec.count) {
                fprintf(stderr, "Format error in line %lld\n",
                        line_count);
                exit(1);
            }
            sbddextended_MyVector_add(&lo_vec, lo);
            sbddextended_MyVector_add(&hi_vec, hi);
        }
    }
    sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);

    if (root_level < 0) {
        root_level = level_count - 1;
    } else if (root_level < level_count - 1) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        exit(1);
    }

    while (bddvarused() < level_vec.count - 1) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc(lo_vec.count * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = (is_zdd == 0 ? bddfalse : bddempty);
    bddnode_buf[1] = (is_zdd == 0 ? bddtrue : bddsingle);

    for (i = (llint)lo_vec.count - 1; i >= sbddextended_BDDNODE_START; --i) {
        for (level = 1; level < (llint)level_vec.count; ++level) {
            if (sbddextended_MyVector_get(&level_vec, level - 1) <= i &&
                i < sbddextended_MyVector_get(&level_vec, level)) {
                break;
            }
        }
        assert(level < (llint)level_vec.count);
        assert((1 <= root_level - level + 1) && ((root_level - level + 1) <= (int)bddvarused()));
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { // BDD
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand(bddnode_buf[sbddextended_MyVector_get(&lo_vec, i)], pfn);
            p1 = bddand(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)], pf);
            bddnode_buf[i] = bddor(p0, p1);
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { // ZDD
            p0 = bddnode_buf[sbddextended_MyVector_get(&lo_vec, i)];
            p1 = bddchange(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)],
                            var);
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


// *************** export functions


sbddextended_INLINE_FUNC
void bddexportbddasknuth_inner(FILE* fp, bddp f, int is_hex
#ifdef __cplusplus
                                    , const WriteObject& sbddextended_writeLine
#endif
                                    )
{
    int clevel, i;
    bddp node, n0, n1;
    llint id0, id1, k;
    bddNodeIndex* index;
    char ss[sbddextended_TEMP_BUFSIZE];

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
    index = bddNodeIndex_makeIndexZWithoutCount(f);

    for (i = index->height; i >= 1; --i) {
        sprintf(ss, "#%d", index->height - i + 1);
        sbddextended_writeLine(ss, fp);
        for (k = 0; k < (llint)index->level_vec_arr[i].count; ++k) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], k);
            n0 = bddgetchild0z(node);
            if (n0 == bddempty) {
                id0 = 0;
            } else if (n0 == bddsingle) {
                id0 = 1;
            } else {
                clevel = (int)bddgetlev(n0);
                if (sbddextended_MyDict_find(&index->node_dict_arr[clevel],
                                                (llint)n0, &id0) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                id0 += index->offset_arr[clevel];
            }
            n1 = bddgetchild1z(node);
            if (n1 == bddempty) {
                id1 = 0;
            } else if (n1 == bddsingle) {
                id1 = 1;
            } else {
                clevel = (int)bddgetlev(n1);
                if (sbddextended_MyDict_find(&index->node_dict_arr[clevel],
                                                (llint)n1, &id1) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                id1 += index->offset_arr[clevel];
            }
            if (is_hex) {
                sprintf(ss, "%llx:%llx,%llx", index->offset_arr[i] + k, id0, id1);
            } else {
                sprintf(ss, "%lld:%lld,%lld", index->offset_arr[i] + k, id0, id1);
            }
            sbddextended_writeLine(ss, fp);
        }
    }
    bddNodeIndex_destruct(index);
    free(index);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void exportBDDAsKnuth(FILE* /*fp*/, const BDD& /*bdd*/, bool /*is_hex*/)
{
    std::cerr << "not implemented yet." << std::endl;
    exit(1);
}

sbddextended_INLINE_FUNC
void exportBDDAsKnuth(std::ostream& /*ost*/, const BDD& /*bdd*/, bool /*is_hex*/)
{
    std::cerr << "not implemented yet." << std::endl;
    exit(1);
}

sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(true, true, &ost);
    bddexportbddasknuth_inner(NULL, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void bddexportbddasknuth(FILE* /*fp*/, bddp /*f*/, int /*is_hex*/)
{
    std::cerr << "not implemented yet." << std::endl;
    exit(1);
}

sbddextended_INLINE_FUNC
void bddexportzbddasknuth(FILE* fp, bddp f, int is_hex)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, f, (is_hex ? 1 : 0), wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasknuth(FILE* fp, bddp f, int is_hex)
{
    unused(fp);
    unused(f);
    unused(is_hex);
    fprintf(stderr, "not implemented yet.\n");
    exit(1);
}

sbddextended_INLINE_FUNC
void bddexportzbddasknuth(FILE* fp, bddp f, int is_hex)
{
    bddexportbddasknuth_inner(fp, f, is_hex);
}

#endif

// *************** import functions

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion_inner(FILE* fp, int root_level, int is_zdd
#ifdef __cplusplus
                                            , ReadLineObject& sbddextended_readLine
#endif
                                            )
{
    int c, level, max_level = 0;
    llint i, id, lo, hi, value, line_count = 0;
    llint max_node_id = 0, root_node_id = 0;
    bddvar var;
    char buf[sbddextended_MAX_LINE];
    char buf1[sbddextended_MAX_LINE];
    char buf2[sbddextended_MAX_LINE];
    char buf3[sbddextended_MAX_LINE];
    char buf4[sbddextended_MAX_LINE];
    bddp p, p0, p1, pf, pfn;
    sbddextended_MyVector node_vec, level_vec, lo_vec, hi_vec;
    sbddextended_MyDict node_dict;

    sbddextended_readLine(buf, fp); // read first line
    if (buf[0] == '.') { // end of file
        return bddnull;
    } else if (buf[0] == 'B' && (buf[1] == '\0' || isspace((int)buf[1]))) {
        return (is_zdd == 0 ? bddfalse : bddempty);
    } else if (buf[0] == 'T' && (buf[1] == '\0' || isspace((int)buf[1]))) {
        return (is_zdd == 0 ? bddtrue : bddsingle);
    }

    sbddextended_MyVector_initialize(&node_vec);
    sbddextended_MyVector_initialize(&level_vec);
    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    do {
        ++line_count;
        if (buf[0] == '.') { // end of file
            break;
        }
        c = sscanf(buf, "%s %s %s %s", buf1, buf2, buf3, buf4);
        if (c < 4) {
            fprintf(stderr, "Format error in line %lld\n", line_count);
            return bddnull;
        }
        if (buf3[0] == 'B') {
            lo = 0;
        } else if (buf3[0] == 'T') {
            lo = 1;
        } else {
            lo = strtoll(buf3, NULL, 10) + 2;
        }
        if (buf4[0] == 'B') {
            hi = 0;
        } else if (buf4[0] == 'T') {
            hi = 1;
        } else {
            hi = strtoll(buf4, NULL, 10) + 2;
        }
        id = strtoll(buf1, NULL, 10) + 2;
        level = (int)strtol(buf2, NULL, 10);
        sbddextended_MyVector_add(&node_vec, id);
        sbddextended_MyVector_add(&level_vec, (llint)level);
        sbddextended_MyVector_add(&lo_vec, lo);
        sbddextended_MyVector_add(&hi_vec, hi);
        if (max_node_id < id) {
            max_node_id = id;
        }
        if (max_level < level) {
            max_level = level;
        }
    } while (sbddextended_readLine(buf, fp));

    if (root_level < 0) {
        root_level = max_level;
    } else if (root_level < max_level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        return bddnull;
    }

    while ((int)bddvarused() < max_level) {
        bddnewvar();
    }

    sbddextended_MyDict_initialize(&node_dict);
    sbddextended_MyDict_add(&node_dict, 0ll, (is_zdd == 0 ? bddfalse : bddempty));
    sbddextended_MyDict_add(&node_dict, 1ll, (is_zdd == 0 ? bddtrue : bddsingle));

    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        level = (int)sbddextended_MyVector_get(&level_vec, i);
        lo = sbddextended_MyVector_get(&lo_vec, i);
        hi = sbddextended_MyVector_get(&hi_vec, i);
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { // BDD
            pf = bddprime(var);
            pfn = bddnot(pf);
            sbddextended_MyDict_find(&node_dict, lo, &value);
            p0 = bddand((bddp)value, pfn);
            sbddextended_MyDict_find(&node_dict, hi, &value);
            p1 = bddand((bddp)value, pf);
            sbddextended_MyDict_add(&node_dict, id, (llint)bddor(p0, p1));
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { // ZDD
            sbddextended_MyDict_find(&node_dict, lo, &value);
            p0 = (bddp)value;
            sbddextended_MyDict_find(&node_dict, hi, &value);
            p1 = bddchange((bddp)value, var);
            sbddextended_MyDict_add(&node_dict, id, (llint)bddunion(p0, p1));
            bddfree(p1);
        }
        root_node_id = id; // The root node is the last node.
    }
    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        if (id != root_node_id) {
            sbddextended_MyDict_find(&node_dict, id, &value);
            bddfree((bddp)value);
        }
    }

    sbddextended_MyDict_find(&node_dict, root_node_id, &value);
    p = (bddp)value;

    sbddextended_MyDict_deinitialize(&node_dict);
    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);
    sbddextended_MyVector_deinitialize(&node_vec);

    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
BDD importBDDAsGraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasgraphillion_inner(fp, root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD importBDDAsGraphillion(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasgraphillion_inner(NULL, root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsGraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasgraphillion_inner(fp, root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsGraphillion(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasgraphillion_inner(NULL, root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasgraphillion_inner(fp, root_level, 0, glo);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasgraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasgraphillion_inner(fp, root_level, 1, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion(FILE* fp, int root_level)
{
    return bddimportbddasgraphillion_inner(fp, root_level, 0);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasgraphillion(FILE* fp, int root_level)
{
    return bddimportbddasgraphillion_inner(fp, root_level, 1);
}

#endif

// *************** export functions


sbddextended_INLINE_FUNC
void bddexportbddasgraphillion_inner(FILE* fp, bddp f,
                                        bddNodeIndex* node_index, int is_zbdd,
                                        int root_level
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

    if (node_index != NULL) {
        if (node_index->is_raw != 0) {
            fprintf(stderr, "The node index must be constructed in the raw mode ");
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

    if (f == bddempty) {
        sbddextended_writeLine("B", fp);
        sbddextended_writeLine(".", fp);
        return;
    } else if (f == bddsingle) {
        sbddextended_writeLine("T", fp);
        sbddextended_writeLine(".", fp);
        return;
    }
    if (root_level < 0) {
        root_level = (int)bddlevofvar(bddtop(f));
    }

    for (i = 1; i <= node_index->height; ++i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
            n = sprintf(ss, "%lld %d", (llint)node, (root_level - i + 1));
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildg(node, k, is_zbdd, 0);
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
        bddNodeIndex_destruct(node_index);
        free(node_index);
    }
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(FILE* fp, const BDD& bdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphillion_inner(fp, bdd.GetID(), bnode_index, 0, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(FILE* fp, const BDD& bdd, int root_level = -1)
{
    exportBDDAsGraphillion<int>(fp, bdd, root_level, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(std::ostream& ost, const BDD& bdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphillion_inner(NULL, bdd.GetID(), bnode_index, 0, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(std::ostream& ost, const BDD& bdd, int root_level = -1)
{
    exportBDDAsGraphillion<int>(ost, bdd, root_level, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(FILE* fp, const ZBDD& zbdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphillion_inner(fp, zbdd.GetID(), bnode_index, 1, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(FILE* fp, const ZBDD& zbdd, int root_level = -1)
{
    exportZBDDAsGraphillion<int>(fp, zbdd, root_level, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(std::ostream& ost, const ZBDD& zbdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphillion_inner(NULL, zbdd.GetID(), bnode_index, 1, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(std::ostream& ost, const ZBDD& zbdd, int root_level = -1)
{
    exportZBDDAsGraphillion<int>(ost, zbdd, root_level, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    WriteObject wo(false, true, NULL);
    // can be used for BDD/ZBDD
    bddexportbddasgraphillion_inner(fp, f, node_index, -1, root_level, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    WriteObject wo(false, true, NULL);
    // only for ZBDD
    bddexportbddasgraphillion_inner(fp, f, node_index, 1, root_level, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    // can be used for BDD/ZBDD
    bddexportbddasgraphillion_inner(fp, f, node_index, -1, root_level);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    // only for ZBDD
    bddexportbddasgraphillion_inner(fp, f, node_index, 1, root_level);
}

#endif

/* *************** import functions */

/* Frees the working vectors of bddimportbddasgraphillion_inner and */
/* returns bddnull from it. Used only there and undefined after it. */
#define sbddextended_freeVectorsAndReturnNull() \
    do { \
        sbddextended_MyVector_deinitialize(&hi_vec); \
        sbddextended_MyVector_deinitialize(&lo_vec); \
        sbddextended_MyVector_deinitialize(&level_vec); \
        sbddextended_MyVector_deinitialize(&node_vec); \
        return bddnull; \
    } while (0)

/* Frees the working data of the node construction loop of */
/* bddimportbddasgraphillion_inner and returns bddnull from it. */
/* Used only there and undefined after it. */
#define sbddextended_freeNodesAndReturnNull() \
    do { \
        for (j = 0; j < i; ++j) { \
            if (sbddextended_MyDict_find(&node_dict, \
                    sbddextended_MyVector_get(&node_vec, j), &value) != 0) { \
                bddfree((bddp)value); \
            } \
        } \
        sbddextended_MyDict_deinitialize(&node_dict); \
        sbddextended_freeVectorsAndReturnNull(); \
    } while (0)

/* Parses the whole token as a decimal node ID in [0, LLONG_MAX - 2] and
   stores the internal ID (the parsed value plus 2). Returns 0 if the token
   contains extra characters, is out of range, or the addition of 2 would
   overflow. */
sbddextended_INLINE_FUNC
int bddimportbddasgraphillion_readid(const char* token, llint* id)
{
    char* end;
    llint v;
    errno = 0;
    end = NULL;
    v = strtoll(token, &end, 10);
    if (end == token || *end != '\0' || errno == ERANGE
            || v < 0 || v > LLONG_MAX - 2) {
        return 0;
    }
    *id = v + 2;
    return 1;
}

/* Parses a child token of the graphillion format: "B" (0-terminal),
   "T" (1-terminal), or a node ID. Stores the internal ID (0, 1, or the
   parsed ID plus 2). Returns 0 on failure. */
sbddextended_INLINE_FUNC
int bddimportbddasgraphillion_readchild(const char* token, llint* id)
{
    if (token[0] == 'B' && token[1] == '\0') {
        *id = 0;
        return 1;
    }
    if (token[0] == 'T' && token[1] == '\0') {
        *id = 1;
        return 1;
    }
    return bddimportbddasgraphillion_readid(token, id);
}

/* Parses the whole token as a decimal level in [1, INT_MAX].
   Returns 0 on failure. */
sbddextended_INLINE_FUNC
int bddimportbddasgraphillion_readlevel(const char* token, int* level)
{
    char* end;
    llint v;
    errno = 0;
    end = NULL;
    v = strtoll(token, &end, 10);
    if (end == token || *end != '\0' || errno == ERANGE
            || v < 1 || v > INT_MAX) {
        return 0;
    }
    *level = (int)v;
    return 1;
}

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion_inner(FILE* fp, int root_level, int is_zdd
#ifdef __cplusplus
                                            , ReadLineObject& sbddextended_readLine
#endif
                                            )
{
    int c, level, max_level = 0;
    llint i, j, id, lo, hi, value, line_count = 0;
    llint lo_value, hi_value;
    llint max_node_id = 0, root_node_id = 0;
    bddvar var;
    char buf[sbddextended_BUFSIZE];
    char buf1[sbddextended_BUFSIZE];
    char buf2[sbddextended_BUFSIZE];
    char buf3[sbddextended_BUFSIZE];
    char buf4[sbddextended_BUFSIZE];
    char buf5[sbddextended_BUFSIZE];
    bddp p, p0, p1, pf, pfn;
    sbddextended_MyVector node_vec, level_vec, lo_vec, hi_vec;
    sbddextended_MyDict node_dict;

    if (!sbddextended_readLine(buf, fp)) { /* read first line */
        fprintf(stderr, "Unexpected end of the input.\n");
        return bddnull;
    }
    if (buf[0] == '.') { /* end of file */
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
        if (buf[0] == '.') { /* end of file */
            break;
        }
        /* Each line must consist of exactly four tokens: the node ID,
           the level, the 0-child, and the 1-child. */
        c = sscanf(buf, "%s %s %s %s %s", buf1, buf2, buf3, buf4, buf5);
        if (c != 4
                || bddimportbddasgraphillion_readid(buf1, &id) == 0
                || bddimportbddasgraphillion_readlevel(buf2, &level) == 0
                || bddimportbddasgraphillion_readchild(buf3, &lo) == 0
                || bddimportbddasgraphillion_readchild(buf4, &hi) == 0) {
            fprintf(stderr, "Format error in line %lld\n", line_count);
            sbddextended_freeVectorsAndReturnNull();
        }
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
        sbddextended_freeVectorsAndReturnNull();
    }

    while (bddvarused() < (bddvar)root_level) {
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
        /* The children must have been already registered because the nodes */
        /* are sorted from the terminals to the root in this format. */
        if (sbddextended_MyDict_find(&node_dict, lo, &lo_value) == 0) {
            fprintf(stderr, "The 0-child (%lld) of the node %lld is not found.\n",
                    lo - 2, id - 2);
            sbddextended_freeNodesAndReturnNull();
        }
        if (sbddextended_MyDict_find(&node_dict, hi, &hi_value) == 0) {
            fprintf(stderr, "The 1-child (%lld) of the node %lld is not found.\n",
                    hi - 2, id - 2);
            sbddextended_freeNodesAndReturnNull();
        }
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { /* BDD */
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand((bddp)lo_value, pfn);
            p1 = bddand((bddp)hi_value, pf);
            sbddextended_MyDict_add(&node_dict, id, (llint)bddor(p0, p1));
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { /* ZDD */
            p0 = (bddp)lo_value;
            p1 = bddchange((bddp)hi_value, var);
            sbddextended_MyDict_add(&node_dict, id, (llint)bddunion(p0, p1));
            bddfree(p1);
        }
        root_node_id = id; /* The root node is the last node. */
    }
    /* The root node is always registered because node_vec is not empty. */
    if (sbddextended_MyDict_find(&node_dict, root_node_id, &value) == 0) {
        fprintf(stderr, "The root node %lld is not found.\n", root_node_id - 2);
        sbddextended_freeNodesAndReturnNull();
    }
    p = (bddp)value;

    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        if (id != root_node_id &&
                sbddextended_MyDict_find(&node_dict, id, &value) != 0) {
            bddfree((bddp)value);
        }
    }

    sbddextended_MyDict_deinitialize(&node_dict);
    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);
    sbddextended_MyVector_deinitialize(&node_vec);

    return p;
}

#undef sbddextended_freeNodesAndReturnNull
#undef sbddextended_freeVectorsAndReturnNull

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

/* *************** export functions */


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
    char ss[sbddextended_BUFSIZE];

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
            n = sbddextended_snprintf2(ss, sbddextended_BUFSIZE, "%lld %d", (llint)node, (root_level - i + 1));
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildg(node, k, is_zbdd, 0);
                if (!bddisterminal(child)) {
                    n += sbddextended_snprintf1(ss + sbddextended_bufPos(n),
                        sbddextended_bufRest(n), " %lld", (llint)child);
                } else if (child == bddempty) {
                    n += sbddextended_snprintf0(ss + sbddextended_bufPos(n),
                        sbddextended_bufRest(n), " B");
                } else if (child == bddsingle) {
                    n += sbddextended_snprintf0(ss + sbddextended_bufPos(n),
                        sbddextended_bufRest(n), " T");
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
    /* can be used for BDD/ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, -1, root_level, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    WriteObject wo(false, true, NULL);
    /* only for ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, 1, root_level, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    /* can be used for BDD/ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, -1, root_level);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    /* only for ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, 1, root_level);
}

#endif

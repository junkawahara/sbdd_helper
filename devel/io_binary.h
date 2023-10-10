/* *************** import functions */

sbddextended_INLINE_FUNC
bddp bddimportbddasbinary_inner(FILE* fp, int root_level, int is_zbdd
#ifdef __cplusplus
                                , ReadCharObject& sbddextended_readUint8
                                , ReadCharObject& sbddextended_readUint16
                                , ReadCharObject& sbddextended_readUint32
                                , ReadCharObject& sbddextended_readUint64
#endif
                                )
{
    ullint i, level, max_level, root_id, number_of_nodes;
    ullint node_count, node_sum;
    unsigned int number_of_terminals;
    bddvar var;
    bddp f, f0, f1;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec;
    unsigned char use_negative_arcs;
    unsigned char v8;
    unsigned short v16;
    ullint v64;

    /* read head 'B' 'D' 'D' */
    for (i = 0; i < 3; ++i) {
        sbddextended_readUint8(&v8, fp);
        if ((i == 0 && v8 != 'B') || (i >= 1 && v8 != 'D')) {
            fprintf(stderr, "This binary is not in the BDD binary format.\n");
            return bddnull;
        }
    }
    sbddextended_readUint8(&v8, fp); /* version */
    if (v8 != 1) {
        fprintf(stderr, "This function supports only version 1.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); /* type */
    if (is_zbdd < 0 && v8 == 1) {
        fprintf(stderr, "Need to specify BDD or ZBDD.\n");
        return bddnull;
    } else if (is_zbdd > 0 && v8 == 2) {
        fprintf(stderr, "The binary indicates that it is BDD, but we interpret it as a ZBDD.\n");
    } else if (is_zbdd == 0 && v8 == 3) {
        fprintf(stderr, "The binary indicates that it is ZDDD, but we interpret it as a BDD.\n");
    }

    sbddextended_readUint16(&v16, fp); /* number_of_arcs */
    if (v16 != 2) {
        fprintf(stderr, "Currently, this function supports only 2 branches.\n");
        return bddnull;
    }

    sbddextended_readUint32(&number_of_terminals, fp); /* number_of_terminals */
    if (number_of_terminals != 2) {
        fprintf(stderr, "Currently, this function supports only 2 terminals.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); /* number_of_bits_for_level */
    if (v8 != 16) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_level == 16.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); /* number_of_bits_for_id */
    if (v8 != 64) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_id == 64.\n");
        return bddnull;
    }

    sbddextended_readUint8(&use_negative_arcs, fp); /* use_negative_arcs */
    sbddextended_readUint64(&max_level, fp); /* max_level */

    if (root_level < 0) {
        root_level = (int)max_level;
    } else if (root_level < (int)max_level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "at least the height of the ZBDD.\n");
        return bddnull;
    }

    sbddextended_readUint64(&v64, fp); /* number_of_roots */
    if (v64 != 1) {
        fprintf(stderr, "Currently, this function supports only 1 root.\n");
        return bddnull;
    }
    /* reserved */
    for (i = 0; i < 8; ++i) {
        sbddextended_readUint64(&v64, fp);
    }

    if (max_level == 0) { /* case of a constant function (0/1-terminal) */
        sbddextended_readUint64(&v64, fp);
        if (v64 == 0) {
            return bddempty;
        } else if (v64 == 1) {
            return bddsingle;
        } else {
            fprintf(stderr, "Currently, this function supports only 0/1-terminal.\n");
            return bddnull;
        }
    }

    sbddextended_MyVector_initialize(&level_vec);

    /* level 0, unused (dummy) */
    sbddextended_MyVector_add(&level_vec, 0ll);

    number_of_nodes = number_of_terminals;
    for (level = 1; level <= max_level; ++level) {
        sbddextended_readUint64(&v64, fp);
        sbddextended_MyVector_add(&level_vec, (llint)v64);
        number_of_nodes += v64;
    }
    sbddextended_readUint64(&root_id, fp);

    /*bddnode_buf = (bddp*)malloc((number_of_nodes + 1) * sizeof(bddp)); */
    bddnode_buf = (bddp*)malloc((size_t)number_of_nodes * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = bddempty;
    bddnode_buf[1] = bddsingle;

    while (bddvarused() < (bddvar)root_level) {
        bddnewvar();
    }

    for (node_count = number_of_terminals;
            node_count < number_of_nodes; ++node_count) {
        /* read 0-child */
        if (!sbddextended_readUint64(&v64, fp)) {
            break;
        }
        /*fprintf(stderr, "%lld\n", v64); */

        /*fprintf(stderr, "0-child: %lld\n", v64); */
        if (v64 <= 1) {
            f0 = bddgetterminal((int)v64, is_zbdd);
        } else {
            if (use_negative_arcs != 0) {
                if(v64 % 2 == 1) {
                    fprintf(stderr, "0-child must not be negative.\n");
                    return bddnull;
                }
                f0 = bddnode_buf[v64 >> 1];
            } else {
                f0 = bddnode_buf[v64];
            }
        }

        /* read 1-child */
        if (!sbddextended_readUint64(&v64, fp)) {
            fprintf(stderr, "illegal format\n");
            return bddnull;
        }
        if (v64 <= 1) {
            f1 = bddgetterminal((int)v64, is_zbdd);
        } else {
            if (use_negative_arcs != 0) {
                f1 = bddnode_buf[v64 >> 1];
                if (v64 % 2 == 1) {
                    f1 = bddtakenot(f1);
                }
            } else {
                f1 = bddnode_buf[v64];
            }
        }

        /* obtain node's level */
        node_sum = number_of_terminals;
        for (level = 1; level <= max_level; ++level) {
            /* add the number of nodes at the level */
            node_sum += (ullint)sbddextended_MyVector_get(&level_vec, (llint)level);
            /*fprintf(stderr, "node_sum: %lld\n", node_sum); */
            if (node_sum > node_count) {
                break;
            }
        }
        assert(level <= max_level);
        var = bddvaroflev((bddvar)((int)level + root_level - (int)max_level));
        assert(node_count < number_of_nodes + 1);
        if (is_zbdd != 0) {
            bddnode_buf[node_count] = bddmakenodez(var, f0, f1);
        } else {
            bddnode_buf[node_count] = bddmakenodeb(var, f0, f1);
        }
    }
    if (use_negative_arcs != 0) {
        if (root_id % 2 == 1) { /* negative arc */
            f = bddtakenot(bddnode_buf[root_id >> 1]);
        } else {
            f = bddnode_buf[root_id >> 1];
        }
    } else {
        f = bddnode_buf[root_id];
    }
    /* FIX ME: need to free of bddnode_buf[*] */
    return f;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
BDD importBDDAsBinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasbinary_inner(fp,
                                    root_level, 0,
                                    glo, glo, glo, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD importBDDAsBinary(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasbinary_inner(NULL,
                                    root_level, 0,
                                    glo, glo, glo, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsBinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasbinary_inner(fp,
                                    root_level, 1,
                                    glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsBinary(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasbinary_inner(NULL,
                                    root_level, 1,
                                    glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddimportbddasbinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasbinary_inner(fp,
                                        root_level, 0,
                                        glo, glo, glo, glo);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasbinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasbinary_inner(fp,
                                        root_level, 1,
                                        glo, glo, glo, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddimportbddasbinary(FILE* fp, int root_level)
{
    return bddimportbddasbinary_inner(fp, root_level, 0);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasbinary(FILE* fp, int root_level)
{
    return bddimportbddasbinary_inner(fp, root_level, 1);
}

#endif


/* *************** export functions */


sbddextended_INLINE_FUNC
void bddexportbddasbinary_inner(FILE* fp, bddp f,
                                bddNodeIndex* node_index,
                                int is_zbdd,
                                int use_negative_arcs
#ifdef __cplusplus
                                , const WriteObject& sbddextended_writeUint8
                                , const WriteObject& sbddextended_writeUint16
                                , const WriteObject& sbddextended_writeUint32
                                , const WriteObject& sbddextended_writeUint64
#endif
                                )
{
    /* Since only BDD/ZDD is treated in the current version, */
    /* the number of terminals is fixed to be 2. */
    const unsigned int number_of_terminals = 2;
    ullint i, j;
    int k, is_making_index = 0;
    ullint max_level;
    ullint number_of_nodes = 0;
    ullint root_id;
    llint id;
    ullint* id_prefix;
    bddp node, child, rchild;

    if (node_index != NULL) {
        if (node_index->is_raw != 0 && use_negative_arcs == 0) {
            fprintf(stderr, "The node index must not be constructed in the raw mode "
                            "when not using negative arcs.\n");
            return;
        } else if (node_index->is_raw == 0 && use_negative_arcs != 0) {
            fprintf(stderr, "The node index must be constructed in the raw mode "
                            "when using negative arcs.\n");
            return;
        }
    }

    max_level = (ullint)bddgetlev(f);

    /* start header */

    sbddextended_writeUint8((unsigned char)'B', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);

    sbddextended_writeUint8((unsigned char)1u, fp); /* version */
    /* DD type */
    if (is_zbdd < 0) { /* can be interpreted as BDD/ZBDD */
        sbddextended_writeUint8((unsigned char)1u, fp);
    } else if (is_zbdd == 0) { /* BDD */
        sbddextended_writeUint8((unsigned char)2u, fp);
    } else { /* ZBDD */
        sbddextended_writeUint8((unsigned char)3u, fp);
    }
    /* number_of_arcs */
    sbddextended_writeUint16((unsigned short)2u, fp);
    /* number_of_terminals */
    sbddextended_writeUint32(number_of_terminals, fp);
    /* number_of_bits_for_level */
    sbddextended_writeUint8((unsigned char)16u, fp);
    /* number_of_bits_for_id */
    sbddextended_writeUint8((unsigned char)64u, fp);
    /* use_negative_arcs */
    if (use_negative_arcs != 0) {
        sbddextended_writeUint8((unsigned char)1u, fp);
    } else {
        sbddextended_writeUint8((unsigned char)0u, fp);
    }
    /* max_level */
    sbddextended_writeUint64(max_level, fp);
    /* number_of_roots */
    sbddextended_writeUint64((ullint)1u, fp);

    /* reserved */
    for (i = 0; i < 8; ++i) {
        sbddextended_writeUint64((ullint)0u, fp);
    }

    /* end header */

    if (is_zbdd < 0) {
        is_zbdd = (!bddisbdd(f) ? 1 : 0);
    }

    if (max_level == 0) { /* case of a constant function (0/1-terminal) */
        if (f == bddempty) {
            sbddextended_writeUint64((ullint)0ull, fp);
        } else if (f == bddsingle) {
            sbddextended_writeUint64((ullint)1ull, fp);
        } else {
            assert(0);
        }
        return;
    }

    if (node_index == NULL) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            if (use_negative_arcs != 0) {
                node_index = bddNodeIndex_makeRawIndexZWithoutCount(f);
            } else {
                node_index = bddNodeIndex_makeIndexZWithoutCount(f);
            }
        } else {
            if (use_negative_arcs != 0) {
                node_index = bddNodeIndex_makeRawIndexBWithoutCount(f);
            } else {
                node_index = bddNodeIndex_makeIndexBWithoutCount(f);
            }
        }
    }

    assert((ullint)node_index->height == max_level);

    /* write the number of nodes in level i and compute the number of nodes */
    for (i = 1; i <= max_level; ++i) {
        sbddextended_writeUint64((ullint)node_index->level_vec_arr[i].count, fp);
        number_of_nodes += (ullint)node_index->level_vec_arr[i].count;
    }

    id_prefix = (ullint*)malloc((size_t)(max_level + 1) * sizeof(ullint));
    if (id_prefix == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    id_prefix[1] = number_of_terminals;
    for (i = 1; i < max_level; ++i) {
        id_prefix[i + 1] = id_prefix[i] + (ullint)node_index->level_vec_arr[i].count;
    }

    /* write the number of the root id */
    /* (In the current version, assume that the number of roots is 2.) */
    root_id = (ullint)number_of_terminals + number_of_nodes - 1;
    assert(root_id == id_prefix[max_level]);

    if (use_negative_arcs != 0) {
        root_id *= 2;
        if (bddisnegative(f)) {
            ++root_id;
        }
    }

    sbddextended_writeUint64(root_id, fp);

    for (i = 1; i <= max_level; ++i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildg(node, k, node_index->is_zbdd, node_index->is_raw);
                if (child == bddempty) {
                    sbddextended_writeUint64(0llu, fp);
                } else if (child == bddsingle) {
                    sbddextended_writeUint64(1llu, fp);
                } else {
                    rchild = (use_negative_arcs != 0 ? bdderasenot(child) : child);
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[bddgetlev(child)],
                                                    (llint)rchild, &id) == 0) {
                        fprintf(stderr, "node not found\n");
                        exit(1);
                    }
                    id += (llint)id_prefix[bddgetlev(child)];
                    if (use_negative_arcs != 0) {
                        id *= 2;
                        if (bddisnegative(child)) {
                            id += 1;
                        }
                    }
                    sbddextended_writeUint64((ullint)id, fp);
                }
            }
        }
    }

    if (is_making_index) {
        bddNodeIndex_destruct(node_index);
        free(node_index);
    }
    free(id_prefix);
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsBinary(FILE* fp, const BDD& bdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, bdd.GetID(), bnode_index,
                                0, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsBinary(FILE* fp, const BDD& bdd, bool use_negative_arcs = true)
{
    exportBDDAsBinary<int>(fp, bdd, use_negative_arcs, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsBinary(std::ostream& ost, const BDD& bdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasbinary_inner(NULL, bdd.GetID(), bnode_index,
                                0, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsBinary(std::ostream& ost, const BDD& bdd, bool use_negative_arcs = true)
{
    exportBDDAsBinary<int>(ost, bdd, use_negative_arcs, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, zbdd.GetID(), bnode_index,
                                1, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs = true)
{
    exportZBDDAsBinary<int>(fp, zbdd, use_negative_arcs, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsBinary(std::ostream& ost, const ZBDD& zbdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasbinary_inner(NULL, zbdd.GetID(), bnode_index,
                                1, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsBinary(std::ostream& ost, const ZBDD& zbdd, bool use_negative_arcs = true)
{
    exportZBDDAsBinary<int>(ost, zbdd, use_negative_arcs, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, f, node_index,
                                0, use_negative_arcs,
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, f, node_index,
                                1, use_negative_arcs,
                                wo, wo, wo, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    bddexportbddasbinary_inner(fp, f, node_index, 0, use_negative_arcs);
}

sbddextended_INLINE_FUNC
void bddexportzbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    bddexportbddasbinary_inner(fp, f, node_index, 1, use_negative_arcs);
}

#endif

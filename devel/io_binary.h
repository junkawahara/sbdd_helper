
sbddextended_INLINE_FUNC
bddp bddconstructbddfrombinary_inner(FILE* fp, int root_level
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
    bddp p0, p1, q;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec;
    unsigned char use_negative_arcs;
    unsigned char v8;
    unsigned short v16;
    ullint v64;

    // read head 'B' 'D' 'D'
    for (i = 0; i < 3; ++i) {
        sbddextended_readUint8(&v8, fp);
        if ((i == 0 && v8 != 'B') || (i >= 1 && v8 != 'D')) {
            fprintf(stderr, "This binary is not in the BDD binary format.\n");
            return bddnull;
        }
    }
    sbddextended_readUint8(&v8, fp); // version
    if (v8 != 1) {
        fprintf(stderr, "This function supports only version 1.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); // type
    if (!(v8 == 1 || v8 == 3)) {
        fprintf(stderr, "Currently, this version supports only ZDD.\n");
        return bddnull;
    }

    sbddextended_readUint16(&v16, fp); // number_of_arcs
    if (v16 != 2) {
        fprintf(stderr, "Currently, this function supports only 2 branches.\n");
        return bddnull;
    }

    sbddextended_readUint32(&number_of_terminals, fp); // number_of_terminals
    if (number_of_terminals != 2) {
        fprintf(stderr, "Currently, this function supports only 2 terminals.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); // number_of_bits_for_level
    if (v8 != 16) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_level == 16.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); // number_of_bits_for_id
    if (v8 != 64) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_id == 64.\n");
        return bddnull;
    }

    sbddextended_readUint8(&use_negative_arcs, fp); // use_negative_arcs
    sbddextended_readUint64(&max_level, fp); // max_level

    if (root_level < 0) {
        root_level = (int)max_level;
    } else if (root_level < (int)max_level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "at least the height of the ZBDD.\n");
        return bddnull;
    }

    sbddextended_readUint64(&v64, fp); // number_of_roots
    if (v64 != 1) {
        fprintf(stderr, "Currently, this function supports only 1 root.\n");
        return bddnull;
    }
    // reserved
    for (i = 0; i < 8; ++i) {
        sbddextended_readUint64(&v64, fp);
    }

    if (max_level == 0) { // case of a constant function (0/1-terminal)
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

    // level 0, unused (dummy)
    sbddextended_MyVector_add(&level_vec, 0);

    number_of_nodes = number_of_terminals;
    for (level = 1; level <= max_level; ++level) {
        sbddextended_readUint64(&v64, fp);
        sbddextended_MyVector_add(&level_vec, (llint)v64);
        number_of_nodes += v64;
    }
    sbddextended_readUint64(&root_id, fp);

    bddnode_buf = (bddp*)malloc((number_of_nodes + 1) * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = bddempty;
    bddnode_buf[1] = bddsingle;

    node_count = number_of_terminals;
    while (sbddextended_readUint64(&v64, fp)) {
        //fprintf(stderr, "%lld\n", v64);
        if (use_negative_arcs != 0) {
            assert(v64 <= 1 || v64 % 2 == 0);
            v64 >>= 1;
        }
        //fprintf(stderr, "0-child: %lld\n", v64);
        p0 = bddnode_buf[v64];
        if (!sbddextended_readUint64(&v64, fp)) {
            fprintf(stderr, "illegal format\n");
            return bddnull;
        }
        if (use_negative_arcs) {
            q = bddnode_buf[v64 >> 1];
            if (v64 % 2 == 1) {
                q = bddtakenot(q);
            }
        } else {
            q = bddnode_buf[v64];
        }
        //fprintf(stderr, "1-child: %lld\n", v64 >> 1);
        node_sum = number_of_terminals;
        for (level = 1; level <= max_level; ++level) {
            // add the number of nodes at the level
            node_sum += (ullint)sbddextended_MyVector_get(&level_vec, (llint)level);
            //fprintf(stderr, "node_sum: %lld\n", node_sum);
            if (node_sum > node_count) {
                break;
            }
        }
        //fprintf(stderr, "level: %lld\n", level);
        assert(level <= max_level);
        p1 = bddchange(q, (bddvar)((int)level + root_level - (int)max_level));
        bddnode_buf[node_count] = bddunion(p0, p1);
        bddfree(p1);
        ++node_count;
    }
    if (use_negative_arcs) {
        if (root_id % 2 == 1) { // negative arc
            return bddtakenot(bddnode_buf[root_id >> 1]);
        } else {
            return bddnode_buf[root_id >> 1];
        }
    } else {
        return bddnode_buf[root_id];
    }
    return bddnull;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromBinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructbddfrombinary_inner(fp,
                                        root_level, glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromBinary(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfrombinary_inner(NULL,
                                        root_level, glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfrombinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddconstructbddfrombinary_inner(fp,
                                           root_level, glo, glo, glo, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzbddfrombinary(FILE* fp, int root_level)
{
    return bddconstructbddfrombinary_inner(fp, root_level);
}

#endif



sbddextended_INLINE_FUNC
void bddwritezbddtobinary_inner(FILE* fp, bddp f
#ifdef __cplusplus
                          , const WriteObject& sbddextended_writeUint8
                          , const WriteObject& sbddextended_writeUint16
                          , const WriteObject& sbddextended_writeUint32
                          , const WriteObject& sbddextended_writeUint64
#endif
                             )
{
    // Since only BDD/ZDD is treated in the current version,
    // the number of terminals is fixed to be 2.
    const unsigned int number_of_terminals = 2;
    const unsigned char use_negative_arcs = 1;
    ullint i, j;
    int k;
    ullint max_level;
    bddNodeIndex* index;
    ullint number_of_nodes = 0;
    ullint root_id;
    llint id;
    ullint* id_prefix;
    bddp node, child;

    max_level = (ullint)bddgetlev(f);

    // start header

    sbddextended_writeUint8((unsigned char)'B', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);

    sbddextended_writeUint8((unsigned char)1u, fp); // version
    sbddextended_writeUint8((unsigned char)1u, fp); // DD type
    // number_of_arcs
    sbddextended_writeUint16((unsigned short)2u, fp);
    // number_of_terminals
    sbddextended_writeUint32(number_of_terminals, fp);
    // number_of_bits_for_level
    sbddextended_writeUint8((unsigned char)16u, fp);
    // number_of_bits_for_id
    sbddextended_writeUint8((unsigned char)64u, fp);
    // use_negative_arcs
    sbddextended_writeUint8(use_negative_arcs, fp);
    // max_level
    sbddextended_writeUint64(max_level, fp);
    // number_of_roots
    sbddextended_writeUint64((ullint)1u, fp);

    // reserved
    for (i = 0; i < 8; ++i) {
        sbddextended_writeUint64((ullint)0u, fp);
    }

    // end header

    if (max_level == 0) { // case of a constant function (0/1-terminal)
        if (f == bddempty) {
            sbddextended_writeUint64((ullint)0ull, fp);
        } else if (f == bddsingle) {
            sbddextended_writeUint64((ullint)1ull, fp);
        } else {
            assert(0);
        }
        return;
    }

    index = bddNodeIndex_makeRawIndexZWithoutCount(f);

    assert((ullint)index->height == max_level);

    // write the number of nodes in level i and compute the number of nodes
    for (i = 1; i <= max_level; ++i) {
        sbddextended_writeUint64((ullint)index->level_vec_arr[i].count, fp);
        number_of_nodes += (ullint)index->level_vec_arr[i].count;
    }

    id_prefix = (ullint*)malloc((max_level + 1) * sizeof(ullint));
    if (id_prefix == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    id_prefix[1] = number_of_terminals;
    for (i = 1; i < max_level; ++i) {
        id_prefix[i + 1] = id_prefix[i] + (ullint)index->level_vec_arr[i].count;
    }

    // write the number of the root id
    // (In the current version, assume that the number of roots is 2.)
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
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildzraw(node, k);
                if (bddisterminal(child)) {
                    sbddextended_writeUint64((child == bddsingle ? 1llu : 0llu), fp);
                    //fprintf(stderr, "%d-child: %d\n", k, (child == bddsingle ? 1 : 0));
                } else {
                    if (sbddextended_MyDict_find(&index->node_dict_arr[bddgetlev(child)],
                                                 (llint)bdderasenot(child), &id) == 0) {
                        fprintf(stderr, "node not found\n");
                        return;
                    }
                    id += (llint)id_prefix[bddgetlev(child)];
                    if (use_negative_arcs != 0) {
                        id *= 2;
                        if (bddisnegative(child)) {
                            id += 1;
                        }
                    }
                    sbddextended_writeUint64((ullint)id, fp);
                    //fprintf(stderr, "%d-child: %lld\n", k, id);
                }
            }
        }
    }

    bddNodeIndex_destruct(index);
    free(index);
    free(id_prefix);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void writeZBDDToBinary(FILE* fp, const ZBDD& zbdd)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtobinary_inner(fp, zbdd.GetID(), wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDToBinary(std::ostream& ost, const ZBDD& zbdd)
{
    WriteObject wo(true, true, &ost);
    bddwritezbddtobinary_inner(NULL, zbdd.GetID(), wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void bddwritezbddtobinary(FILE* fp, bddp f)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtobinary_inner(fp, f, wo, wo, wo, wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritezbddtobinary(FILE* fp, bddp f)
{
    bddwritezbddtobinary_inner(fp, f);
}

#endif

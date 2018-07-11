sbddextended_INLINE_FUNC
void bddprintzbddelements_inner(FILE* fp, bddp f, const char* delim1,
                                const char* delim2, const char* var_name_map[]
#ifdef __cplusplus
                       , const WriteObject& sbddextended_write
#endif
                          )
{
    bddp* bddnode_stack;
    bddp g;
    char* op_stack;
    char op;
    int i, height, sp, is_first_delim1, is_first_delim2;
    bddp node, child;
    char buf[sbddextended_MAX_LINE];

    if (f == bddnull) {
        sbddextended_write("N", fp);
        return;
    } else if (f == bddempty) {
        sbddextended_write("E", fp);
        return;
    } else if (f == bddsingle) {
        return;
    }

    is_first_delim1 = 1;
    if (bddisnegative(f)) {
        // Output bddsingle first
        is_first_delim1 = 0;
        g = bddtakenot(f);
    } else {
        g = f;
    }

    height = (int)bddgetlev(g) + 1;
    bddnode_stack = (bddp*)malloc((size_t)height * sizeof(bddp));
    if (bddnode_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc((size_t)height * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    sp = 0;
    bddnode_stack[sp] = g;
    op_stack[sp] = 0;

    while (sp >= 0) {
        node = bddnode_stack[sp];
        op = op_stack[sp];
        if (node == bddempty) {
            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        } else if (node == bddsingle) {
            if (is_first_delim1 != 0) {
                is_first_delim1 = 0;
            } else {
                sprintf(buf, "%s", delim1);
                sbddextended_write(buf, fp);
            }
            is_first_delim2 = 1;
            for (i = 0; i < sp; ++i) {
                if (op_stack[i] == 0) {
                    if (is_first_delim2 != 0) {
                        is_first_delim2 = 0;
                    } else {
                        sprintf(buf, "%s", delim2);
                        sbddextended_write(buf, fp);
                    }
                    if (var_name_map != NULL) {
                        sprintf(buf, "%s", var_name_map[bddgetvar(bddnode_stack[i])]);
                    } else {
                        sprintf(buf, "%d", bddgetvar(bddnode_stack[i]));
                    }
                    sbddextended_write(buf, fp);
                }
            }

            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        } else {
            if (op <= 1) {
                if (op == 0) {
                    child = bddgetchild1z(node);
                } else {
                    child = bddgetchild0z(node);
                }
                ++sp;
                bddnode_stack[sp] = child;
                op_stack[sp] = 0;
            } else {
                --sp;
                if (sp < 0) {
                    break;
                }
                ++op_stack[sp];
            }
        }
    }
    free(op_stack);
    free(bddnode_stack);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void PrintZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, wo);
}

sbddextended_INLINE_FUNC
void PrintZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(false, false, NULL);
    const char** arr = sbddextended_strvector_to_array(var_name_map);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void PrintZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, wo);
}

sbddextended_INLINE_FUNC
void PrintZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(true, false, &ost);
    const char** arr = sbddextended_strvector_to_array(var_name_map);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, wo);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, wo);
}

#else

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map);
}

#endif

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth_inner(FILE* fp, int is_hex, int root_level,
                                        int is_zdd
#ifdef __cplusplus
                             , const ReadLineObject& sbddextended_readLine
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
BDD ConstructBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD ConstructBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(true, &ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    return bddconstructbddfromfileknuth_inner(fp, is_hex,
                                              root_level, 0, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level)
{
    return bddconstructbddfromfileknuth_inner(fp, is_hex, root_level, 0);
}

#endif


#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD ConstructZBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD ConstructZBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(true, &ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    return bddconstructbddfromfileknuth_inner(fp, is_hex,
                                              root_level, 1, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level)
{
    return bddconstructbddfromfileknuth_inner(fp, is_hex, root_level, 1);
}

#endif

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth_inner(FILE* fp, bddp f, int is_hex
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
void WriteZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtofileknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void WriteZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(true, true, &ost);
    bddwritezbddtofileknuth_inner(NULL, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth(FILE* fp, bddp f, int is_hex)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtofileknuth_inner(fp, f, (is_hex ? 1 : 0), wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth(FILE* fp, bddp f, int is_hex)
{
    bddwritezbddtofileknuth_inner(fp, f, is_hex);
}

#endif

void bddoutputbddforgraphviz(FILE* fp, bddp f, bddNodeIndex* index)
{
    int i, k, c, clevel;
    llint cvalue;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    if (index == NULL) {
        is_making_index = 1;
        index = bddNodeIndex_makeIndexBWithoutCount(f);
    }

    fprintf(fp, "digraph {\n");
    fprintf(fp, "\tt0 [label = \"0\", shape = box];\n");
    fprintf(fp, "\tt1 [label = \"1\", shape = box];\n");

    fprintf(fp, "\tr%d [style = invis]\n", index->height);
    for (i = index->height; i >= 1; --i) {
        fprintf(fp, "\tr%d [style = invis];\n", i - 1);
        fprintf(fp, "\tr%d -> r%d [style = invis];\n", i, i - 1);
    }

    for (i = index->height; i >= 1; --i) {
        //fprintf(stderr, "%d", i);
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            //fprintf(stderr, " %d", node);
            fprintf(fp, "\tv%d_%lld [shape = circle];\n", i, (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                //fprintf(stderr, " %d", k);
                child = bddgetchildb(node, k);
                if (!bddisterminal(child)) {
                    clevel = (int)bddgetlev(child);
                    c = sbddextended_MyDict_find(&index->node_dict_arr[clevel],
                                                 (llint)child, &cvalue);
                    assert(c != 0);

                    fprintf(fp, "\tv%d_%lld -> v%d_%lld", i, (llint)j,
                            clevel, cvalue);
                    if (k == 0) {
                        fprintf(fp, " [style = dotted]");
                    }
                    fprintf(fp, ";\n");
                } else {
                    fprintf(fp, "\tv%d_%lld -> t%d", i, (llint)j,
                            (child == bddfalse ? 0 : 1));
                    if (k == 0) {
                        fprintf(fp, " [style = dotted]");
                    }
                    fprintf(fp, ";\n");
                }
            }
        }
        fprintf(fp, "\t{rank = same; r%d; ", i);
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            fprintf(fp, "v%d_%lld; ", i, (llint)j);
        }
        fprintf(fp, "}\n");
    }

    fprintf(fp, "\t{rank = same; r0; t0; t1; }\n");
    fprintf(fp, "}\n");

    if (is_making_index) {
        bddNodeIndex_destruct(index);
    }
}

// table size should be at least 2^n and vararr size should be at least n
bddp bddtruthtabletobdd(const unsigned char* table, const bddvar* vararr, int n)
{
    ullint n_pow, i;
    int j;
    bddp f, g, h, gp;
    if (n > 64) {
        fprintf(stderr, "n >= 64 not supported.\n");
        exit(1);
    }

    f = bddfalse;
    n_pow = (1llu << n);
    for (i = 0; i < n_pow; ++i) {
        if (table[i] != 0) {
            g = bddtrue;
            for (j = 0; j < n; ++j) {
                gp = bddprime(vararr[j]);
                if (((i >> j) & 1) == 0) {
                    h = bddnot(gp);
                    bddfree(gp);
                    gp = h;
                }
                h = bddand(g, gp);
                bddfree(g);
                bddfree(gp);
                g = h;
            }
            h = bddor(f, g);
            bddfree(f);
            bddfree(g);
            f = h;
        }
    }
    return f;
}


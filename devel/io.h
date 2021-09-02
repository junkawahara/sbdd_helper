// num_of_variables: 0 -> elements format, non 0 -> value list format
sbddextended_INLINE_FUNC
void bddprintzbddelements_inner(FILE* fp, bddp f, const char* delim1,
                                const char* delim2, const char* var_name_map[],
                                int num_of_variables
#ifdef __cplusplus
                       , const WriteObject& sbddextended_write
#endif
                          )
{
    bddp* bddnode_stack;
    bddp g;
    char* op_stack;
    char* value_list;
    char op;
    int i, height, sp, is_first_delim1, is_first_delim2;
    bddvar v;
    bddp node, child;
    char buf[sbddextended_MAX_LINE];

    if (f == bddnull) {
        sbddextended_write("N", fp);
        return;
    } else if (f == bddempty) {
        sbddextended_write("E", fp);
        return;
    } else if (f == bddsingle) {
        if (num_of_variables != 0) {
            for (i = 1; i <= num_of_variables; ++i) {
                sprintf(buf, "0");
                sbddextended_write(buf, fp);
                if (i < num_of_variables) {
                    sprintf(buf, "%s", delim2);
                    sbddextended_write(buf, fp);
                }
            }
        }
        return;
    }

    is_first_delim1 = 1;
    if (bddisnegative(f)) {
        // Output bddsingle first
        if (num_of_variables != 0) {
            for (i = 1; i <= num_of_variables; ++i) {
                sprintf(buf, "0");
                sbddextended_write(buf, fp);
                if (i < num_of_variables) {
                    sprintf(buf, "%s", delim2);
                    sbddextended_write(buf, fp);
                }
            }
        }
        is_first_delim1 = 0;
        g = bddtakenot(f);
    } else {
        g = f;
    }

    height = (int)bddgetlev(g);
    bddnode_stack = (bddp*)malloc((size_t)(height + 1) * sizeof(bddp));
    if (bddnode_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc((size_t)(height + 1) * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    if (num_of_variables != 0) {
        value_list = (char*)malloc((size_t)(num_of_variables + 1) * sizeof(char));
        if (value_list == NULL) {
            fprintf(stderr, "out of memory\n");
            return;
        }
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
            if (num_of_variables != 0) {
                for (i = 1; i <= num_of_variables; ++i) {
                    value_list[i] = 0;
                }
                for (i = 0; i < sp; ++i) {
                    if (op_stack[i] == 0) {
                        v = bddgetvar(bddnode_stack[i]);
                        if (! (1 <= v && v <= (bddvar)num_of_variables)) {
                            fprintf(stderr, "variable number is "
                                "not in {1,...,num_of_variables} \n");
                            return;
                        }
                        value_list[v] = 1;
                    }
                }
                for (i = 1; i <= num_of_variables; ++i) {
                    sprintf(buf, "%d", value_list[i]);
                    sbddextended_write(buf, fp);
                    if (i < num_of_variables) {
                        sprintf(buf, "%s", delim2);
                        sbddextended_write(buf, fp);
                    }
                }
            } else {
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
    if (num_of_variables != 0) {
        free(value_list);
    }
    free(op_stack);
    free(bddnode_stack);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(false, false, NULL);
    const char** arr = sbddextended_strVectorToArray(var_name_map);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, 0, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(true, false, &ost);
    const char** arr = sbddextended_strVectorToArray(var_name_map);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, 0, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd)
{
    fprintf(fp, "{");
    printZBDDElements(fp, zbdd, "}, {", ", ");
    fprintf(fp, "}");
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd)
{
    ost << "{";
    printZBDDElements(ost, zbdd, "}, {", ", ");
    ost << "}";
}

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, 0, wo);
}

#else

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, 0);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, 0);
}

#endif


#ifdef __cplusplus

sbddextended_INLINE_FUNC
void printZBDDElementsAsValueList(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, int num_of_variables)
{
    if (num_of_variables < 0) {
        fprintf(stderr, "num_of_variables must be at least 1\n");
        return;
    }

    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(),
                               delim2.c_str(), NULL, num_of_variables, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElementsAsValueList(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, int num_of_variables)
{
    if (num_of_variables < 0) {
        std::cerr << "num_of_variables must be at least 1" << std::endl;
        return;
    }

    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, num_of_variables, wo);
}

#endif

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth_inner(FILE* fp, int is_hex, int root_level,
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
BDD constructBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD constructBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
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
ZBDD constructZBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
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
bddp bddconstructzbddfromelements_inner_getoneset(const char* line, int line_len,
                                                  const char* small_sep,
                                                  int small_sep_len)
{
    char int_buf[sbddextended_MAX_LINE];
    int mode = 0;
    int pos;
    int v;
    int sep_pos = 0;
    int sep_start = 0;
    int num_start = 0;
    bddvar vararr[256];
    int vararr_size = 0;

    if (line_len == 0) {
        return bddsingle;
    }

    for (pos = 0; pos < line_len + 1; ++pos) {
        if (pos < line_len) {
            if (mode == 0) {
                if (line[pos] == small_sep[0]) {
                    mode = 1;
                    sep_start = pos;
                    sep_pos = 1;
                }
            } else if (mode == 1) {
                if (line[pos] == small_sep[sep_pos]) {
                    ++sep_pos;
                } else {
                    mode = 0;
                }
            }
        }
        if (pos >= line_len || (mode == 1 && sep_pos >= small_sep_len)) {
            if (pos >= line_len) {
                sep_start = pos;
            }
            assert(num_start < sep_start);
            strncpy(int_buf, line + num_start, (size_t)(sep_start - num_start));
            int_buf[sep_start - num_start] = '\0';
            if (sscanf(int_buf, "%d", &v) != 1) {
                fprintf(stderr, "format error in "
                        "bddconstructzbddfromelements_inner_getoneset\n");
            }
            vararr[vararr_size] = (bddvar)v;
            ++vararr_size;
            num_start = pos + 1;
        }
    }

    //printf("vararr_size = %d\n", vararr_size);
    //for (v = 0; v < vararr_size; ++v) {
    //    printf("<%d> ", vararr[v]);
    //}
    //printf("\n");

    return bddgetsingleset(vararr, vararr_size);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements_inner(FILE* fp, const char* large_sep,
                                        const char* small_sep
#ifdef __cplusplus
                             , ReadCharObject& sbddextended_readChar
#endif
                             )
{
    char buf[sbddextended_MAX_LINE];
    bddp p, q, r;
    int pos;
    int start_pos;
    int sep_pos;
    int c;
    int large_sep_len;
    int small_sep_len;
    int mode;

    p = bddfalse;
    large_sep_len = (int)strlen(large_sep);
    if (large_sep_len <= 0) {
        fprintf(stderr, "large_sep_len must be at least one");
        exit(1);
    }
    small_sep_len = (int)strlen(small_sep);
    if (small_sep_len <= 0) {
        fprintf(stderr, "small_sep_len must be at least one");
        exit(1);
    }

    start_pos = 0;
    sep_pos = 0;
    mode = 0;
    pos = 0;
    while (1 == 1) {
        c = sbddextended_readChar(fp);
        if (c != -1) {
            if (mode == 0) {
                if (c == large_sep[0]) {
                    mode = 1;
                    sep_pos = 1;
                } else {
                    if (pos - start_pos >= sbddextended_MAX_LINE) {
                        fprintf(stderr, "length must not exceed sbddextended_MAX_LINE");
                        exit(1);
                    }
                    buf[pos - start_pos] = (char)c;
                    ++pos;
                }
            } else if (mode == 1) {
                if (c == large_sep[sep_pos]) {
                    ++sep_pos;
                } else {
                    if (pos - start_pos + sep_pos >= sbddextended_MAX_LINE) {
                        fprintf(stderr, "length must not exceed sbddextended_MAX_LINE");
                        exit(1);
                    }
                    memcpy(buf + pos - start_pos, large_sep, (size_t)sep_pos);
                    pos += sep_pos;
                    sep_pos = 0;
                }
            }
        }
        if (c == -1 || sep_pos >= large_sep_len) {
            buf[pos - start_pos] = '\0';

            //fprintf(stderr,"<<%s>>\n", buf);
            // obtain buf
            q = bddconstructzbddfromelements_inner_getoneset(buf,
                                                             pos - start_pos,
                                                             small_sep,
                                                             small_sep_len);
            r = bddunion(p, q);
            bddfree(p);
            bddfree(q);
            p = r;
            if (c == -1) {
                break;
            }
            start_pos = pos + sep_pos + 1;
            pos += sep_pos + 1;
            mode = 0;
            sep_pos = 0;
        }
    }
    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(FILE* fp, const char* large_sep,
                              const char* small_sep)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructzbddfromelements_inner(fp, large_sep, small_sep,
                                           glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(std::istream& ist, const char* large_sep,
                              const char* small_sep)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructzbddfromelements_inner(NULL, large_sep, small_sep,
                                           glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp, const char* large_sep,
                                  const char* small_sep)
{
    ReadLineObject glo;
    return bddconstructzbddfromelements_inner(fp, large_sep, small_sep,
                                              glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp, const char* large_sep,
                                  const char* small_sep)
{
    return bddconstructzbddfromelements_inner(fp, large_sep, small_sep);
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
void writeZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtofileknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void writeZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex)
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

void bddoutputbddforgraphviz(FILE* fp, bddp f, bddNodeIndex* index, int is_zbdd)
{
    int i, k, c, clevel;
    llint cvalue;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    if (index == NULL) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    fprintf(fp, "digraph {\n");
    fprintf(fp, "\tt0 [label = \"0\", shape = box, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, height = 0.6, fontsize = 24];\n");
    fprintf(fp, "\tt1 [label = \"1\", shape = box, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, height = 0.6, fontsize = 24];\n");

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
            fprintf(fp, "\tv%d_%lld [shape = circle, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, label = \"\"];\n", i, (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                //fprintf(stderr, " %d", k);
                if (is_zbdd != 0) {
                    child = bddgetchildz(node, k);
                } else {
                    child = bddgetchildb(node, k);
                }
                if (!bddisterminal(child)) {
                    clevel = (int)bddgetlev(child);
                    c = sbddextended_MyDict_find(&index->node_dict_arr[clevel],
                                                 (llint)child, &cvalue);
                    assert(c != 0);

                    fprintf(fp, "\tv%d_%lld -> v%d_%lld", i, (llint)j,
                            clevel, cvalue);
                    fprintf(fp, " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        fprintf(fp, ", style = dotted");
                    }
                    fprintf(fp, "];\n");
                } else {
                    fprintf(fp, "\tv%d_%lld -> t%d", i, (llint)j,
                            (child == bddfalse ? 0 : 1));
                    fprintf(fp, " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        fprintf(fp, ", style = dotted");
                    }
                    fprintf(fp, "];\n");
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


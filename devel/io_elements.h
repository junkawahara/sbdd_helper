// *************** import functions

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements_inner(FILE* fp
#ifdef __cplusplus
                    , ReadCharObject& sbddextended_readChar
#endif
                                        )
{
    bddp p, q, r;
    bddvar* vararr;
    int vararr_pos, vararr_size;
    int c, prev_c, v;
    int mode, first;

    vararr_size = sbddextended_MAX_LINE;
    vararr = (bddvar*)malloc((size_t)vararr_size * sizeof(bddvar));
    if (vararr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    vararr_pos = 0;

    p = bddfalse;
    first = 1;

    mode = 0; // 0: skip ws, 1: reading nums
    v = 0;
    c = 0;
    while (c != -1) {
        prev_c = c;
        c = sbddextended_readChar(fp);
        if (first == 1) {
            first = 0;
            if (c == 'T') {
                p = bddtrue;
                break;
            } else if (c == 'B' || c == 'E' || c == 'F') {
                break;
            }
        }
        if (c != -1) {
            if (!isdigit(c) && c != '\n' && !isspace(c)) {
                fprintf(stderr, "invalid char\n");
                exit(1);
            }
            if (mode == 0) {
                if (isdigit(c)) {
                    v = c - '0';
                    mode = 1;
                }
            } else if (mode == 1) {
                if (isdigit(c)) {
                    v = 10 * v + (c - '0');
                }
            }
        }
        if ((c == -1 || c == '\n' || isspace(c)) && mode == 1) {
            if (vararr_pos >= vararr_size) {
                vararr_size *= 2;
                vararr = (bddvar*)realloc(vararr, (size_t)vararr_size * sizeof(bddvar));
                if (vararr == NULL) {
                    fprintf(stderr, "out of memory\n");
                    exit(1);
                }
            }
            vararr[vararr_pos] = (bddvar)v;
            ++vararr_pos;
            mode = 0;
        }
        if ((c == -1 && prev_c != '\n') || c == '\n') {
            q = bddgetsingleset(vararr, vararr_pos);
            r = bddunion(p, q);
            bddfree(p);
            bddfree(q);
            p = r;
            vararr_pos = 0;
        }
    }
    free(vararr);
    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(FILE* fp)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructzbddfromelements_inner(fp, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(std::istream& ist)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructzbddfromelements_inner(NULL, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp)
{
    ReadLineObject glo;
    return bddconstructzbddfromelements_inner(fp, glo);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(FILE* /*fp*/, const char* /*large_sep*/,
                                const char* /*small_sep*/)
{
    std::cerr << "Arguments large_sep and small_sep are no longer supported.";
    exit(1);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(std::istream& /*ist*/, const char* /*large_sep*/,
                                const char* /*small_sep*/)
{
    std::cerr << "Arguments large_sep and small_sep are no longer supported.";
    exit(1);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* /*fp*/, const char* /*large_sep*/,
                                    const char* /*small_sep*/)
{
    std::cerr << "Arguments large_sep and small_sep are no longer supported.";
    exit(1);
}

#else

// The following function is no longer supported.
//sbddextended_INLINE_FUNC
//bddp bddconstructzbddfromelements(FILE* fp, const char* large_sep,
//                                    const char* small_sep)
//{
//    return bddconstructzbddfromelements_inner(fp, large_sep, small_sep);
//}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp)
{
    return bddconstructzbddfromelements_inner(fp);
}

#endif


// *************** export functions


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
    char* value_list = NULL;
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

sbddextended_INLINE_FUNC
std::string ZStr(const ZBDD& zbdd)
{
    if (zbdd == ZBDD(-1)) { // null
        return std::string("N");
    } if (zbdd == ZBDD(0)) { // 0-terminal
        return std::string("E");
    } else {
        std::ostringstream ost;
        printZBDDElements(ost, zbdd);
        return ost.str();
    }
}

sbddextended_INLINE_FUNC
std::string zstr(const ZBDD& zbdd)
{
    return ZStr(zbdd);
}

#endif

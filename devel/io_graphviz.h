
sbddextended_INLINE_FUNC
void bddwritebddforgraphviz_inner(FILE* fp, bddp f,
                                   bddNodeIndex* index, int is_zbdd
#ifdef __cplusplus
                        , const WriteObject& sbddextended_writeLine
#endif
                             )
{
    int i, k, c, clevel, n;
    llint cvalue;
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

    sbddextended_writeLine("digraph {", fp);
    // print terminals
    if (f != bddsingle) {
        sbddextended_writeLine("\tt0 [label = \"0\", shape = box, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, height = 0.6, fontsize = 24];", fp);
    }
    if (f != bddempty) {
        sbddextended_writeLine("\tt1 [label = \"1\", shape = box, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, height = 0.6, fontsize = 24];", fp);
    }
    if (f == bddempty || f == bddsingle) {
        sbddextended_writeLine("}", fp);
        return;
    }

    // print vars and levels
    sprintf(ss, "\tr%d [shape = plaintext, label = \"var level\"]", index->height + 1);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "\tr%d [shape = plaintext, label = \"%4d%7d\"]", index->height, bddvaroflev((bddvar)index->height), index->height);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "\tr%d -> r%d [style = invis];", index->height + 1, index->height);
    sbddextended_writeLine(ss, fp);
    for (i = index->height; i >= 1; --i) {
        if (i > 1) {
            sprintf(ss, "\tr%d [shape = plaintext, label = \"%4d%7d\"];", i - 1, bddvaroflev((bddvar)(i - 1)), i - 1);
            sbddextended_writeLine(ss, fp);
        } else {
            sbddextended_writeLine("\tr0 [style = invis];", fp);
        }
        sprintf(ss, "\tr%d -> r%d [style = invis];", i, i - 1);
        sbddextended_writeLine(ss, fp);
    }

    for (i = index->height; i >= 1; --i) {
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            sprintf(ss, "\tv%d_%lld [shape = circle, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, label = \"\"];", i, (llint)j);
            sbddextended_writeLine(ss, fp);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
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

                    n = sprintf(ss, "\tv%d_%lld -> v%d_%lld", i, (llint)j,
                                clevel, cvalue);
                    n += sprintf(ss + n, " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        n += sprintf(ss + n, ", style = dotted");
                    }
                    sprintf(ss + n, "];");
                    sbddextended_writeLine(ss, fp);
                } else {
                    n = sprintf(ss, "\tv%d_%lld -> t%d", i, (llint)j,
                            (child == bddfalse ? 0 : 1));
                    n += sprintf(ss + n, " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        n += sprintf(ss + n, ", style = dotted");
                    }
                    sprintf(ss + n, "];");
                    sbddextended_writeLine(ss, fp);
                }
            }
        }
        n = sprintf(ss, "\t{rank = same; r%d; ", i);
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            n += sprintf(ss + n, "v%d_%lld; ", i, (llint)j);
            if (j % 10 == 9 && j < index->level_vec_arr[i].count - 1) {
                sbddextended_writeLine(ss, fp);
                n = sprintf(ss, "\t\t");
            }
        }
        n += sprintf(ss + n, "}");
        sbddextended_writeLine(ss, fp);
    }

    sbddextended_writeLine("\t{rank = same; r0; t0; t1; }", fp);
    sbddextended_writeLine("}", fp);

    if (is_making_index) {
        bddNodeIndex_destruct(index);
    }
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void writeBDDForGraphviz(FILE* fp, const BDD& bdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphviz_inner(fp, bdd.GetID(), bindex, 0, wo);
}

sbddextended_INLINE_FUNC
void writeBDDForGraphviz(std::ostream& ost, const BDD& bdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddwritebddforgraphviz_inner(NULL, bdd.GetID(), bindex, 0, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(FILE* fp, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphviz_inner(fp, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(std::ostream& ost, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddwritebddforgraphviz_inner(NULL, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void bddwritebddforgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* index)
{
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphviz_inner(fp, f, index, -1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritebddforgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* index)
{
    bddwritebddforgraphviz_inner(fp, f, index, -1);
}

#endif

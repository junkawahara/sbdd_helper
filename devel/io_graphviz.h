
sbddextended_INLINE_FUNC
void bddexportbddasgraphviz_inner(FILE* fp, bddp f,
                                    bddNodeIndex* node_index, int is_zbdd
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

    if (node_index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            node_index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            node_index = bddNodeIndex_makeIndexBWithoutCount(f);
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
    sprintf(ss, "\tr%d [shape = plaintext, label = \"var level\"]", node_index->height + 1);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "\tr%d [shape = plaintext, label = \"%4d%7d\"]", node_index->height, bddvaroflev((bddvar)node_index->height), node_index->height);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "\tr%d -> r%d [style = invis];", node_index->height + 1, node_index->height);
    sbddextended_writeLine(ss, fp);
    for (i = node_index->height; i >= 1; --i) {
        if (i > 1) {
            sprintf(ss, "\tr%d [shape = plaintext, label = \"%4d%7d\"];", i - 1, bddvaroflev((bddvar)(i - 1)), i - 1);
            sbddextended_writeLine(ss, fp);
        } else {
            sbddextended_writeLine("\tr0 [style = invis];", fp);
        }
        sprintf(ss, "\tr%d -> r%d [style = invis];", i, i - 1);
        sbddextended_writeLine(ss, fp);
    }

    for (i = node_index->height; i >= 1; --i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
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
                    c = sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
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
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            n += sprintf(ss + n, "v%d_%lld; ", i, (llint)j);
            if (j % 10 == 9 && j < node_index->level_vec_arr[i].count - 1) {
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
        bddNodeIndex_destruct(node_index);
    }
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(FILE* fp, const BDD& bdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphviz_inner(fp, bdd.GetID(), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(FILE* fp, const BDD& bdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsGraphviz<int>(fp, bdd, option, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(std::ostream& ost, const BDD& bdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphviz_inner(NULL, bdd.GetID(), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(std::ostream& ost, const BDD& bdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsGraphviz<int>(ost, bdd, option, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(FILE* fp, const ZBDD& zbdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphviz_inner(fp, zbdd.GetID(), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(FILE* fp, const ZBDD& zbdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsGraphviz<int>(fp, zbdd, option, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(std::ostream& ost, const ZBDD& zbdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphviz_inner(NULL, zbdd.GetID(), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(std::ostream& ost, const ZBDD& zbdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsGraphviz<int>(ost, zbdd, option, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    // can be used for BDD/ZBDD
    bddexportbddasgraphviz_inner(fp, f, node_index, -1, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphviz(FILE* fp, bddp f,
                                bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    // only for ZBDD
    bddexportbddasgraphviz_inner(fp, f, node_index, 1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* node_index)
{
    // can be used for BDD/ZBDD
    bddexportbddasgraphviz_inner(fp, f, node_index, -1);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* node_index)
{
    // only for ZBDD
    bddexportbddasgraphviz_inner(fp, f, node_index, 1);
}

#endif

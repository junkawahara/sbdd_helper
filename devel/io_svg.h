
#ifdef __cplusplus // currently svg is supported only in C++ version

struct ExportAsSvg_arcinfo {
public:
    bddp f;
    int arc; // 0 or 1-arc
    int fposx;
    int fposy;

    ExportAsSvg_arcinfo(bddp ff, int a, int fpx, int fpy)
        : f(ff), arc(a), fposx(fpx), fposy(fpy) { }
};

bool ExportAsSvg_arcinfo_compare(const ExportAsSvg_arcinfo& a1,
                                    const ExportAsSvg_arcinfo& a2)
{
    return a1.fposx < a2.fposx;
}

int ExportAsSvg_getCirclePosX(int x, int r, double rad)
{
    return x + static_cast<int>(r * cos(rad));
}

int ExportAsSvg_getCirclePosY(int y, int r, double rad)
{
    return y - static_cast<int>(r * sin(rad));
}

template<typename T>
sbddextended_INLINE_FUNC
void bddexportassvg_inner(FILE* fp, bddp f,
                            DDIndex<T>* index, int is_zbdd
//#ifdef __cplusplus
                            , const WriteObject& sbddextended_writeLine
//#endif
                        )
{
    char ss[sbddextended_BUFSIZE];
    const int node_radius = 20;
    const int node_interval_x = 30;
    const int node_interval_y = 40;
    const int terminal_x = 30;
    const int terminal_y = 40;
    const int margin_x = 20;
    const int margin_y = 20;
    const int label_y = 7;
    const int arc_width = 3;
    bool draw_zero = true;

    bool is_made = false;
    if (index == NULL) {
        index = new DDIndex<T>(f);
        is_made = true;
    }

    std::map<bddp, std::pair<int, int> > pos_map;
    std::map<bddp, std::vector<ExportAsSvg_arcinfo> > dest_info;
    std::map<bddp, std::pair<int, int> > dest0_pos;
    std::map<bddp, std::pair<int, int> > dest1_pos;
    int y = margin_y + node_radius;
    llint max_nodes = 0;
    for (int level = index->height(); level >= 1; --level) {
        if (max_nodes < index->size(level)) {
            max_nodes = index->size(level);
        }
    }
    if (max_nodes < 2) {
        max_nodes = 2;
    }
    const int node_x = static_cast<int>((2 * node_radius + 1)
        * max_nodes
        + node_interval_x * (max_nodes + 1));

    for (int level = index->height(); level >= 1; --level) {
        int num_nodes = static_cast<int>(index->size(level));
        int x = node_x / (num_nodes + 1) - (node_radius + node_interval_x - margin_x);
        for (llint j = 0; j < num_nodes; ++j) {
            bddp g = index->getNode(level, j).getBddp();
            bddp g0 = (is_zbdd ? bddgetchild0z(g) : bddgetchild0b(g));
            bddp g1 = (is_zbdd ? bddgetchild1z(g) : bddgetchild1b(g));
            pos_map[g] = std::make_pair(x, y);
            dest_info[g0].push_back(ExportAsSvg_arcinfo(g, 0, x, y));
            dest_info[g1].push_back(ExportAsSvg_arcinfo(g, 1, x, y));
            x += node_x / (num_nodes + 1);
        }
        y += 2 * node_radius + node_interval_y;
    }
    y += terminal_y / 2 - node_radius;
    const int max_x = static_cast<int>(2 * node_radius * max_nodes
        + node_interval_x * (max_nodes - 1)
        + 2 * margin_x);
    const int max_y = y + terminal_y / 2 + margin_y;
    const int num_terms = (draw_zero ? 2 : 1);
    int tx = node_x / (num_terms + 1) - (node_radius + node_interval_x - margin_x);
    if (draw_zero) {
        pos_map[bddempty] = std::make_pair(tx, y);
        tx += node_x / (num_terms + 1);
    }
    pos_map[bddsingle] = std::make_pair(tx, y);

    std::map<bddp, std::vector<ExportAsSvg_arcinfo> >::iterator itor = dest_info.begin();

    while (itor != dest_info.end()) {
        bddp g = itor->first;
        std::vector<ExportAsSvg_arcinfo>& infovec = itor->second;
        std::sort(infovec.begin(), infovec.end(),
            ExportAsSvg_arcinfo_compare);
        double rad = 2.0 / 3.0 * M_PI;
        if (infovec.size() == 1) {
            rad = 1.0 / 2.0 * M_PI;
        }
        for (int i = 0; i < static_cast<int>(infovec.size()); ++i) {
            int posx = ExportAsSvg_getCirclePosX(pos_map[g].first, node_radius, rad);
            int posy = ExportAsSvg_getCirclePosY(pos_map[g].second, node_radius, rad);
            if (infovec[i].arc == 0) {
                dest0_pos[infovec[i].f] = std::make_pair(posx, posy);
            } else {
                assert(infovec[i].arc == 1);
                dest1_pos[infovec[i].f] = std::make_pair(posx, posy);
            }
            if (infovec.size() >= 2) {
                rad -= M_PI / 3.0 / static_cast<double>(infovec.size() - 1);
            }
        }
        ++itor;
    }

    // draw svg
    sprintf(ss, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\">", max_x, max_y);
    sbddextended_writeLine(ss, fp);

    sbddextended_writeLine("<marker id=\"arrow\" viewBox=\"-10 -4 20 8\" markerWidth=\"10\" markerHeight=\"10\" orient=\"auto\">", fp);
    sbddextended_writeLine("    <polygon points=\"-10,-4 0,0 -10,4\" fill=\"#1b3966\" stroke=\"none\" />", fp);
    sbddextended_writeLine("</marker>", fp);

    // draw nodes
    for (int level = index->height(); level >= 1; --level) {
        for (llint j = 0; j < index->size(level); ++j) {
            bddp g = index->getNode(level, j).getBddp();
            sprintf(ss, "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"#deebf7\" stroke=\"#1b3966\" stroke-width=\"%d\" />", pos_map[g].first, pos_map[g].second, node_radius, arc_width);
            sbddextended_writeLine(ss, fp);
            sprintf(ss, "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" font-size=\"24\">%d</text>", pos_map[g].first, pos_map[g].second + label_y, level);
            sbddextended_writeLine(ss, fp);
        }
    }

    // draw arcs
    for (int level = index->height(); level >= 1; --level) {
        for (llint j = 0; j < index->size(level); ++j) {
            bddp g = index->getNode(level, j).getBddp();
            int posx1 = ExportAsSvg_getCirclePosX(pos_map[g].first, node_radius, 4.0 / 3.0 * M_PI);
            int posy1 = ExportAsSvg_getCirclePosY(pos_map[g].second, node_radius, 4.0 / 3.0 * M_PI);
            int posx2 = dest0_pos[g].first;
            int posy2 = dest0_pos[g].second;
            sprintf(ss, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#1b3966\" stroke-width=\"%d\" stroke-dasharray=\"10,5\"  marker-end=\"url(#arrow)\" />", posx1, posy1, posx2, posy2, arc_width);
            sbddextended_writeLine(ss, fp);
            posx1 = ExportAsSvg_getCirclePosX(pos_map[g].first, node_radius, 5.0 / 3.0 * M_PI);
            posy1 = ExportAsSvg_getCirclePosY(pos_map[g].second, node_radius, 5.0 / 3.0 * M_PI);
            posx2 = dest1_pos[g].first;
            posy2 = dest1_pos[g].second;
            sprintf(ss, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#1b3966\" stroke-width=\"%d\" marker-end=\"url(#arrow)\" />", posx1, posy1, posx2, posy2, arc_width);
            sbddextended_writeLine(ss, fp);
        }
    }

    //draw terminals
    sprintf(ss, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#deebf7\" stroke=\"#1b3966\" stroke-width=\"%d\" />", pos_map[bddempty].first - terminal_x / 2, pos_map[bddempty].second - terminal_y / 2, terminal_x, terminal_y, arc_width);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#deebf7\" stroke=\"#1b3966\" stroke-width=\"%d\" />", pos_map[bddsingle].first - terminal_x / 2, pos_map[bddsingle].second - terminal_y / 2, terminal_x, terminal_y, arc_width);
    sbddextended_writeLine(ss, fp);

    sprintf(ss, "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" font-size=\"24\">0</text>", pos_map[bddempty].first, pos_map[bddempty].second + label_y);
    sbddextended_writeLine(ss, fp);

    sprintf(ss, "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" font-size=\"24\">1</text>", pos_map[bddsingle].first, pos_map[bddsingle].second + label_y);
    sbddextended_writeLine(ss, fp);

    sbddextended_writeLine("</svg>", fp);

    if (is_made) {
        delete index;
        index = NULL;
    }
}

template<typename T>
sbddextended_INLINE_FUNC
void exportBDDAsSvg(FILE* fp, const BDD& bdd,
                    std::map<std::string, std::string>* /*option*/,
                    DDIndex<T>* index)
{
    WriteObject wo(false, true, NULL);
    bddexportassvg_inner(fp, bdd.GetID(), index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsSvg(FILE* fp, const BDD& bdd,
                    std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsSvg<int>(fp, bdd, option, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportBDDAsSvg(std::ostream& ost, const BDD& bdd,
                    std::map<std::string, std::string>* /*option*/,
                    DDIndex<T>* index)
{
    WriteObject wo(true, true, &ost);
    bddexportassvg_inner(NULL, bdd.GetID(), index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsSvg(std::ostream& ost, const BDD& bdd,
                    std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsSvg<int>(ost, bdd, option, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsSvg(FILE* fp, const ZBDD& zbdd,
                        std::map<std::string, std::string>* /*option*/,
                        DDIndex<T>* index)
{
    WriteObject wo(false, true, NULL);
    bddexportassvg_inner(fp, zbdd.GetID(), index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsSvg(FILE* fp, const ZBDD& zbdd,
                        std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsSvg<int>(fp, zbdd, option, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsSvg(std::ostream& ost, const ZBDD& zbdd,
                        std::map<std::string, std::string>* /*option*/,
                        DDIndex<T>* index)
{
    WriteObject wo(true, true, &ost);
    bddexportassvg_inner(NULL, zbdd.GetID(), index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsSvg(std::ostream& ost, const ZBDD& zbdd,
                        std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsSvg<int>(ost, zbdd, option, NULL);
}

#endif

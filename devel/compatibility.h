/* The functions below are the old names of the functions that were renamed
   in the past. They used to be defined as object-like macros, but a macro
   ignores namespaces and rewrites every occurrence of the identifier,
   including the ones in the code of the user; defining them as inline
   functions instead avoids that. The change log of
   documents/reference.md lists the old names and the new ones. */

/* *************************** C version starts **************************** */

#ifdef __cplusplus
sbddextended_INLINE_FUNC
bddp bddconstructzbddfrombinary(FILE* fp, int root_level = -1)
#else
sbddextended_INLINE_FUNC
bddp bddconstructzbddfrombinary(FILE* fp, int root_level)
#endif
{
    return bddimportzbddasbinary(fp, root_level);
}

sbddextended_INLINE_FUNC
void bddwritezbddtobinary(FILE* fp, bddp f, int use_negative_arcs,
                          bddNodeIndex* node_index)
{
    bddexportzbddasbinary(fp, f, use_negative_arcs, node_index);
}

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion(FILE* fp, bddp f, bddNodeIndex* node_index,
                               int root_level)
{
    bddexportbddasgraphillion(fp, f, node_index, root_level);
}

#ifdef __cplusplus
sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
#else
sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level)
#endif
{
    return bddimportbddasknuth(fp, is_hex, root_level);
}

#ifdef __cplusplus
sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
#else
sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level)
#endif
{
    return bddimportzbddasknuth(fp, is_hex, root_level);
}

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth(FILE* fp, bddp f, int is_hex,
                             bddNodeIndex* node_index)
{
    bddexportzbddasknuth(fp, f, is_hex, node_index);
}

sbddextended_INLINE_FUNC
void bddwritebddforgraphviz(FILE* fp, bddp f, bddNodeIndex* node_index)
{
    bddexportbddasgraphviz(fp, f, node_index);
}

/* ************************** C++ version starts *************************** */

#ifdef __cplusplus

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllSetsIncluding(const T& base_variables,
                         const std::vector<bddvar>& target_variables)
{
    return getPowerSetIncluding(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllSetsIncluding(const T& base_variables,
                         const std::set<bddvar>& target_variables)
{
    return getPowerSetIncluding(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllSetsIncluding(const T& base_variables, bddvar v)
{
    return getPowerSetIncluding(base_variables, v);
}

sbddextended_INLINE_FUNC
ZBDD getAllSetsIncluding(int n, const std::vector<bddvar>& target_variables)
{
    return getPowerSetIncluding(n, target_variables);
}

sbddextended_INLINE_FUNC
ZBDD getAllSetsIncluding(int n, const std::set<bddvar>& target_variables)
{
    return getPowerSetIncluding(n, target_variables);
}

sbddextended_INLINE_FUNC
ZBDD getAllSetsIncluding(int n, int v)
{
    return getPowerSetIncluding(n, v);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsIncluding(const T& base_variables,
                              const std::vector<bddvar>& target_variables)
{
    return getPowerSetIncluding(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsIncluding(const T& base_variables,
                              const std::set<bddvar>& target_variables)
{
    return getPowerSetIncluding(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsIncluding(const T& base_variables, bddvar v)
{
    return getPowerSetIncluding(base_variables, v);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsIncluding(int n,
                              const std::vector<bddvar>& target_variables)
{
    return getPowerSetIncluding(n, target_variables);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsIncluding(int n, const std::set<bddvar>& target_variables)
{
    return getPowerSetIncluding(n, target_variables);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsIncluding(int n, int v)
{
    return getPowerSetIncluding(n, v);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsNotIncluding(int n,
                                 const std::vector<bddvar>& target_variables)
{
    return getPowerSetNotIncluding(n, target_variables);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsNotIncluding(int n,
                                 const std::set<bddvar>& target_variables)
{
    return getPowerSetNotIncluding(n, target_variables);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsNotIncluding(int n, int v)
{
    return getPowerSetNotIncluding(n, v);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllSetsWithCard(const T& variables, int k)
{
    return getPowerSetWithCard(variables, k);
}

sbddextended_INLINE_FUNC
ZBDD getAllSetsWithCard(int n, int k)
{
    return getPowerSetWithCard(n, k);
}

template<typename T>
sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsWithCard(const T& variables, int k)
{
    return getPowerSetWithCard(variables, k);
}

sbddextended_INLINE_FUNC
ZBDD getAllPowerSetsWithCard(int n, int k)
{
    return getPowerSetWithCard(n, k);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromBinary(FILE* fp, int root_level = -1)
{
    return importZBDDAsBinary(fp, root_level);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromBinary(std::istream& ist, int root_level = -1)
{
    return importZBDDAsBinary(ist, root_level);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromGraphillion(FILE* fp, int root_level = -1)
{
    return importZBDDAsGraphillion(fp, root_level);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromGraphillion(std::istream& ist, int root_level = -1)
{
    return importZBDDAsGraphillion(ist, root_level);
}

sbddextended_INLINE_FUNC
BDD constructBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    return importBDDAsKnuth(fp, is_hex, root_level);
}

sbddextended_INLINE_FUNC
BDD constructBDDFromFileKnuth(std::istream& ist, bool is_hex,
                              int root_level = -1)
{
    return importBDDAsKnuth(ist, is_hex, root_level);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    return importZBDDAsKnuth(fp, is_hex, root_level);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromFileKnuth(std::istream& ist, bool is_hex,
                                int root_level = -1)
{
    return importZBDDAsKnuth(ist, is_hex, root_level);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDToBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs,
                       DDIndex<T>* node_index)
{
    exportZBDDAsBinary(fp, zbdd, use_negative_arcs, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDToBinary(FILE* fp, const ZBDD& zbdd,
                       bool use_negative_arcs = true)
{
    exportZBDDAsBinary(fp, zbdd, use_negative_arcs);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDToBinary(std::ostream& ost, const ZBDD& zbdd,
                       bool use_negative_arcs, DDIndex<T>* node_index)
{
    exportZBDDAsBinary(ost, zbdd, use_negative_arcs, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDToBinary(std::ostream& ost, const ZBDD& zbdd,
                       bool use_negative_arcs = true)
{
    exportZBDDAsBinary(ost, zbdd, use_negative_arcs);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(FILE* fp, const ZBDD& zbdd, int root_level,
                             DDIndex<T>* node_index)
{
    exportZBDDAsGraphillion(fp, zbdd, root_level, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(FILE* fp, const ZBDD& zbdd, int root_level = -1)
{
    exportZBDDAsGraphillion(fp, zbdd, root_level);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(std::ostream& ost, const ZBDD& zbdd,
                             int root_level, DDIndex<T>* node_index)
{
    exportZBDDAsGraphillion(ost, zbdd, root_level, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(std::ostream& ost, const ZBDD& zbdd,
                             int root_level = -1)
{
    exportZBDDAsGraphillion(ost, zbdd, root_level);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex,
                          DDIndex<T>* node_index)
{
    exportZBDDAsKnuth(fp, zbdd, is_hex, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex = false)
{
    exportZBDDAsKnuth(fp, zbdd, is_hex);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex,
                          DDIndex<T>* node_index)
{
    exportZBDDAsKnuth(ost, zbdd, is_hex, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd,
                          bool is_hex = false)
{
    exportZBDDAsKnuth(ost, zbdd, is_hex);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeBDDForGraphviz(FILE* fp, const BDD& bdd,
                         std::map<std::string, std::string>* option,
                         DDIndex<T>* node_index)
{
    exportBDDAsGraphviz(fp, bdd, option, node_index);
}

sbddextended_INLINE_FUNC
void writeBDDForGraphviz(FILE* fp, const BDD& bdd,
                         std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsGraphviz(fp, bdd, option);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeBDDForGraphviz(std::ostream& ost, const BDD& bdd,
                         std::map<std::string, std::string>* option,
                         DDIndex<T>* node_index)
{
    exportBDDAsGraphviz(ost, bdd, option, node_index);
}

sbddextended_INLINE_FUNC
void writeBDDForGraphviz(std::ostream& ost, const BDD& bdd,
                         std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsGraphviz(ost, bdd, option);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(FILE* fp, const ZBDD& zbdd,
                          std::map<std::string, std::string>* option,
                          DDIndex<T>* node_index)
{
    exportZBDDAsGraphviz(fp, zbdd, option, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(FILE* fp, const ZBDD& zbdd,
                          std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsGraphviz(fp, zbdd, option);
}

template<typename T>
sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(std::ostream& ost, const ZBDD& zbdd,
                          std::map<std::string, std::string>* option,
                          DDIndex<T>* node_index)
{
    exportZBDDAsGraphviz(ost, zbdd, option, node_index);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(std::ostream& ost, const ZBDD& zbdd,
                          std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsGraphviz(ost, zbdd, option);
}

#endif /* __cplusplus */

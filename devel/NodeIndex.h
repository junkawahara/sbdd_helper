typedef struct tagbddNodeIndex {
    int is_raw;
    // All of the following four pointers are NULL if f is a terminal or bddnull.
    sbddextended_MyDict* node_dict_arr;
    sbddextended_MyVector* level_vec_arr; // stores all nodes at level i
    llint* offset_arr;
    llint* count_arr; // array representing the number of solutions for node i
    int height;
    bddp f;
} bddNodeIndex;

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexWithoutCount_inner(bddp f, int is_raw, int is_zdd)
{
    int i, k, level;
    size_t j;
    bddp node, child;
    bddNodeIndex* index;

    index = (bddNodeIndex*)malloc(sizeof(bddNodeIndex));
    if (index == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    index->is_raw = is_raw;
    index->f = f;
    index->height = (int)bddgetlev(f);

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        index->node_dict_arr = NULL;
        index->level_vec_arr = NULL;
        index->offset_arr = NULL;
        index->count_arr = NULL;
        return index;
    }

    if (is_raw) {
        f = bdderasenot(f);
    }

    index->node_dict_arr = (sbddextended_MyDict*)malloc(
                            (size_t)(index->height + 1) * sizeof(sbddextended_MyDict));
    if (index->node_dict_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    index->level_vec_arr = (sbddextended_MyVector*)malloc(
                            (size_t)(index->height + 1) * sizeof(sbddextended_MyVector));
    if (index->level_vec_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (i = 1; i <= index->height; ++i) {
        sbddextended_MyDict_initialize(&index->node_dict_arr[i]);
        sbddextended_MyVector_initialize(&index->level_vec_arr[i]);
    }

    sbddextended_MyDict_add(&index->node_dict_arr[index->height],
                            (llint)f,
                            0); // 0 means the first node in the level
    sbddextended_MyVector_add(&index->level_vec_arr[index->height], (llint)f);

    for (i = index->height; i >= 1; --i) {
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_raw) {
                    if (is_zdd) {
                        child = bddgetchildzraw(node, k);
                    } else {
                        child = bddgetchildbraw(node, k);
                    }
                    child = bdderasenot(child);
                } else {
                    if (is_zdd) {
                        child = bddgetchildz(node, k);
                    } else {
                        child = bddgetchildb(node, k);
                    }
                }

                if (!bddisterminal(child)) {
                    level = (int)bddgetlev(child);
                    if (sbddextended_MyDict_find(&index->node_dict_arr[level],
                                                 (llint)child, NULL) == 0) {
                        sbddextended_MyDict_add(&index->node_dict_arr[level],
                                                (llint)child,
                                                (llint)index->node_dict_arr[level].count);
                        sbddextended_MyVector_add(&index->level_vec_arr[level], (llint)child);
                    }
                }
            }
        }
    }

    index->offset_arr = (llint*)malloc((size_t)(index->height + 1) * sizeof(llint));
    if (index->offset_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    index->offset_arr[index->height] = sbddextended_BDDNODE_START;

    for (i = index->height; i >= 1; --i) {
        index->offset_arr[i - 1] = index->offset_arr[i] + (llint)index->level_vec_arr[i].count;
    }

    index->count_arr = NULL;
    return index;
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexBWithoutCount(bddp f)
{
    return bddNodeIndex_makeIndexWithoutCount_inner(f, 0, 0);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexZWithoutCount(bddp f)
{
    return bddNodeIndex_makeIndexWithoutCount_inner(f, 0, 1);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexWithoutCount(bddp f)
{
    return bddNodeIndex_makeIndexWithoutCount_inner(f, 0, (bddiszbdd(f) ? 1 : 0));
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeRawIndexBWithoutCount(bddp f)
{
    return bddNodeIndex_makeIndexWithoutCount_inner(f, 1, 0);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeRawIndexZWithoutCount(bddp f)
{
    return bddNodeIndex_makeIndexWithoutCount_inner(f, 1, 1);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeRawIndexWithoutCount(bddp f)
{
    return bddNodeIndex_makeIndexWithoutCount_inner(f, 1, (bddiszbdd(f) ? 1 : 0));
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndex_inner(bddp f, int is_raw, int is_zdd)
{
    int i, clevel, raw_flag;
    llint j, id0, id1;
    bddp node, n0, n1;
    bddNodeIndex* index;

    index = bddNodeIndex_makeIndexWithoutCount_inner(f, is_raw, is_zdd);

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        return index;
    }

    if (is_zdd) {
        index->count_arr = (llint*)malloc((size_t)index->offset_arr[0] * sizeof(llint));
        index->count_arr[0] = 0;
        index->count_arr[1] = 1;

        for (i = 1; i <= index->height; ++i) {
            for (j = index->offset_arr[i]; j < index->offset_arr[i - 1]; ++j) {
                node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i],
                                                       j - index->offset_arr[i]);
                if (is_raw) {
                    n0 = bddgetchild0zraw(node);
                } else {
                    n0 = bddgetchild0z(node);
                }
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
                raw_flag = 0;
                if (is_raw) {
                    n1 = bddgetchild1zraw(node);
                    if (bddisnegative(n1)) {
                        raw_flag = 1;
                        n1 = bdderasenot(n1);
                    }
                } else {
                    n1 = bddgetchild1z(node);
                }
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
                index->count_arr[j] = index->count_arr[id0] + index->count_arr[id1];
                if (is_raw && raw_flag) {
                    index->count_arr[j] += 1;
                }
            }
        }
    } else {
        // not implemented yet.
    }
    return index;
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexB(bddp f)
{
    return bddNodeIndex_makeIndex_inner(f, 0, 0);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexZ(bddp f)
{
    return bddNodeIndex_makeIndex_inner(f, 0, 1);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndex(bddp f)
{
    return bddNodeIndex_makeIndex_inner(f, 0, (bddiszbdd(f) ? 1 : 0));
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeRawIndexB(bddp f)
{
    return bddNodeIndex_makeIndex_inner(f, 1, 0);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeRawIndexZ(bddp f)
{
    return bddNodeIndex_makeIndex_inner(f, 1, 1);
}

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeRawIndex(bddp f)
{
    return bddNodeIndex_makeIndex_inner(f, 1, (bddiszbdd(f) ? 1 : 0));
}

sbddextended_INLINE_FUNC
llint bddNodeIndex_size(const bddNodeIndex* index)
{
    if (index->f == bddnull || index->f == bddfalse || index->f == bddtrue) {
        return 0ll;
    }
    return (llint)index->offset_arr[0] - sbddextended_BDDNODE_START;
}

sbddextended_INLINE_FUNC
llint bddNodeIndex_sizeAtLevel(const bddNodeIndex* index, int level)
{
    if (index->f == bddnull || index->f == bddfalse || index->f == bddtrue) {
        return 0ll;
    } else if (level <= 0 || index->height < level) {
        return 0ll;
    }
    return (llint)(index->offset_arr[level - 1] - index->offset_arr[level]);
}

sbddextended_INLINE_FUNC
void bddNodeIndex_sizeEachLevel(const bddNodeIndex* index, bddvar* arr)
{
    int i;
    if (!(index->f == bddnull || index->f == bddfalse || index->f == bddtrue)) {
        for (i = 1; i <= index->height; ++i) {
            arr[i] = (bddvar)(index->offset_arr[i - 1] - index->offset_arr[i]);
        }
    }
}

sbddextended_INLINE_FUNC
llint bddNodeIndex_count(const bddNodeIndex* index)
{
    if (index->f == bddnull || index->f == bddfalse) {
        return 0ll;
    } else if (index->f == bddtrue) {
        return 1ll;
    }
    if (index->is_raw) {
        if (bddisnegative(index->f)) {
            return index->count_arr[sbddextended_BDDNODE_START] + 1;
        } else {
            return index->count_arr[sbddextended_BDDNODE_START];
        }
    } else {
        return index->count_arr[sbddextended_BDDNODE_START];
    }
}

sbddextended_INLINE_FUNC
void bddNodeIndex_destruct(bddNodeIndex* index)
{
    int i;

    if (index != NULL) {
        if (index->level_vec_arr != NULL) {
            for (i = 1; i <= index->height; ++i) {
                sbddextended_MyVector_deinitialize(&index->level_vec_arr[i]);
            }
            free(index->level_vec_arr);
        }
        if (index->node_dict_arr != NULL) {
            for (i = 1; i <= index->height; ++i) {
                sbddextended_MyDict_deinitialize(&index->node_dict_arr[i]);
            }
            free(index->node_dict_arr);
        }
        if (index->offset_arr != NULL) {
            free(index->offset_arr);
        }
        if (index->count_arr != NULL) {
            free(index->count_arr);
        }
    }
}

sbddextended_INLINE_FUNC
void bddNodeIndex_copy(bddNodeIndex* dest,
                       const bddNodeIndex* src)
{
    dest->is_raw = src->is_raw;
    if (src->node_dict_arr != NULL) {
        sbddextended_MyDict_copy(dest->node_dict_arr, src->node_dict_arr);
    } else {
        dest->node_dict_arr = NULL;
    }
    if (src->level_vec_arr != NULL) {
        sbddextended_MyVector_copy(dest->level_vec_arr, src->level_vec_arr);
    } else {
        dest->level_vec_arr = NULL;
    }
    if (src->offset_arr != NULL) {
        memcpy(dest->offset_arr, src->offset_arr, (size_t)(src->height + 1) * sizeof(llint));
    } else {
        dest->offset_arr = NULL;
    }
    if (src->count_arr != NULL) {
        memcpy(dest->count_arr, src->count_arr, (size_t)src->offset_arr[0] * sizeof(llint));
    } else {
        dest->count_arr = NULL;
    }
    dest->height = src->height;
    dest->f = src->f;
}


// *************************** C++ version start *****************************

#ifdef __cplusplus

// This class is obsolate.
class DDNodeIndex {
private:
    bddNodeIndex* index_;

public:
    DDNodeIndex(const BDD& f, bool is_raw = true)
    {
        index_ = bddNodeIndex_makeIndex_inner(f.GetID(), (is_raw ? 1 : 0), 0);
    }

    DDNodeIndex(const ZBDD& f, bool is_raw = true)
    {
        index_ = bddNodeIndex_makeIndex_inner(f.GetID(), (is_raw ? 1 : 0), 1);
    }

    bddNodeIndex* getRawPointer()
    {
        return index_;
    }

    llint size()
    {
        return bddNodeIndex_size(index_);
    }

    llint sizeAtLevel(int level)
    {
        return bddNodeIndex_sizeAtLevel(index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr)
    {
        if (!(index_->f == bddnull || index_->f == bddfalse || index_->f == bddtrue)) {
            arr.resize(index_->height + 1);
            for (int i = 1; i <= index_->height; ++i) {
                arr[i] = (bddvar)(index_->offset_arr[i - 1] - index_->offset_arr[i]);
            }
        }
    }

    llint count()
    {
        return bddNodeIndex_count(index_);
    }

    ~DDNodeIndex()
    {
        bddNodeIndex_destruct(index_);
        free(index_);
    }

    class DDNodeIterator : public std::iterator<std::input_iterator_tag, bddp>
    {
    private:
        const DDNodeIndex& index_;
        size_t pos_;
        size_t level_;

    public:
        DDNodeIterator(const DDNodeIndex& index, bool is_end) : index_(index), pos_(0)
        {
            if (is_end) {
                level_ = 0; // This means pointing at end;
            } else {
                level_ = index.index_->height;
            }
        }

        bddp operator*() const
        {
            if (level_ <= 0) {
                return bddfalse;
            }
            return sbddextended_MyVector_get(&index_.index_->
                                              level_vec_arr[level_],
                                             (llint)pos_);
        }

        DDNodeIterator& operator++()
        {
            if (level_ > 0) {
                ++pos_;
                while (level_ > 0 &&
                       pos_ >= index_.index_->level_vec_arr[level_].count) {
                    pos_ = 0;
                    --level_;
                }
            }
            return *this;
        }

        bool operator==(const DDNodeIterator& it) const
        {
            if (level_ <= 0) {
                return it.level_ <= 0;
            } else {
                return pos_ == it.pos_ && level_ == it.level_;
            }
        }

        bool operator!=(const DDNodeIterator& it) const
        {
            return !(operator==(it));
        }
    };

    DDNodeIterator begin()
    {
        return DDNodeIterator(*this, false);
    }

    DDNodeIterator end()
    {
        return DDNodeIterator(*this, true);
    }

};

template <typename T> class DDIndex;

template <typename T = int>
class DDNode {
private:
    const bddp f_;
    DDIndex<T>& index_;
public:
    T& value;

public:
    DDNode(bddp f, DDIndex<T>& index) : f_(f), index_(index), value(index.getStorageRef(f)) { }

    bddp getBddp() const
    {
        return f_;
    }

    DDNode child(int c) const
    {
        bddp g;
        if (isTerminal()) {
            g = f_;
        } else if (bddisbdd(f_)) {
            g = bddgetchildb(f_, c);
        } else {
            g = bddgetchildz(f_, c);
        }
        DDNode node(g, index_);
        return node;
    }

    bool isTerminal() const
    {
        return (!!bddisterminal(f_));
    }

    bool isTerminal(int t) const
    {
        if (t == 0 && f_ == bddfalse) {
            return true;
        } else if (t == 1 && f_ == bddtrue) {
            return true;
        } else {
            return false;
        }
    }
};

template <typename T = int>
bool operator==(const DDNode<T>& node1, const DDNode<T>& node2)
{
    return node1.getBddp() == node2.getBddp();
}

template <typename T = int>
bool operator!=(const DDNode<T>& node1, const DDNode<T>& node2)
{
    return node1.getBddp() != node2.getBddp();
}

template <typename T = int>
class DDIndex {
private:
    bddNodeIndex* index_;
    std::map<bddp, T> storage_;
    bool is_count_made = false;

#ifdef USE_GMP
    std::map<bddp, mpz_class> count_storage_;
#else
    std::map<bddp, llint> count_storage_;
#endif

    void initialize(bddp f, bool /*is_raw*/, int is_zbdd)
    {
        // currently, we do not support raw mode. We set is_raw to be false.
        //index_ = bddNodeIndex_makeIndexWithoutCount_inner(f, (is_raw ? 1 : 0), is_zbdd);
        index_ = bddNodeIndex_makeIndexWithoutCount_inner(f, 0, is_zbdd);
    }

    llint optimize(const std::vector<llint>& weights, bool is_max,
                    std::set<bddvar>& s) const
    {
        if (index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        // llint -> max/min value, bool -> 1-arc if true, 0-arc if false
        std::map<bddp, std::pair<llint, bool> > sto;

        if (is_max) {
            sto[bddempty].first = -static_cast<llint>((1ull << 63) - 1);
        } else {
            sto[bddempty].first = static_cast<llint>((1ull << 63) - 1);
        }
        sto[bddsingle].first = 0;

        for (int level = 1; level <= height(); ++level) {
            for (llint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                if (is_max) {
                    if (sto[child1].first == sto[bddempty].first
                            || sto[child0].first > sto[child1].first + weights[var]) {
                        sto[f].first = sto[child0].first;
                        sto[f].second = false; // 0-arc side
                    } else {
                        sto[f].first = sto[child1].first + weights[var];
                        sto[f].second = true; // 1-arc side
                    }
                } else {
                    if (sto[child1].first == sto[bddempty].first
                            || sto[child0].first > sto[child1].first + weights[var]) {
                        sto[f].first = sto[child1].first + weights[var];
                        sto[f].second = true; // 1-arc side
                    } else {
                        sto[f].first = sto[child0].first;
                        sto[f].second = false; // 0-arc side
                    }
                }
            }
        }
        bddp g = index_->f;
        while (!bddisterminal(g)) {
            if (sto[g].second) { // 1-arc
                s.insert(bddgetvar(g));
                g = bddgetchild1z(g);
            } else { // 0-arc
                g = bddgetchild0z(g);
            }
        }
        assert(g == bddsingle);
        return sto[index_->f].first;
    }

#ifdef USE_GMP
    llint getStorageValue(bddp f) const
    {
        return static_cast<llint>(count_storage_.at(f).get_ui());
    }
#else
    llint getStorageValue(bddp f) const
    {
        return count_storage_.at(f);
    }
#endif

    llint getOrderNumber(bddp f, std::set<bddvar>& s) const
    {
        bddp f0;

        if (f == bddempty) {
            return -1;
        } else if (f == bddsingle) {
            if (s.size() > 0) {
                return -1;
            } else {
                return 0;
            }
        }

        if (s.size() == 0) {
            if (bddisemptymember(f)) {
                return 0;
            } else {
                return -1;
            }
        }

        llint value = 0;
        if (bddisemptymember(f)) {
            value += 1;
            f0 = bdderasenot(f);
        } else {
            f0 = f;
        }

        bddvar var = bddgetvar(f);
        if (s.count(var) >= 1) {
            s.erase(var);
            llint v = getOrderNumber(bddgetchild1z(f), s);
            if (v >= 0) {
                value += v;
            } else {
                value = -1;
            }
        } else {
            llint v = getStorageValue(bddgetchild1z(f))
                        + getOrderNumber(bddgetchild0z(f0), s);
            if (v >= 0) {
                value += v;
            } else {
                value = -1;
            }
        }
        return value;
    }

#ifdef USE_GMP
    mpz_class getOrderNumberMP(bddp f, std::set<bddvar>& s) const
    {
        bddp f0;

        if (f == bddempty) {
            return mpz_class(-1);
        } else if (f == bddsingle) {
            if (s.size() > 0) {
                return mpz_class(-1);
            } else {
                return mpz_class(0);
            }
        }

        if (s.size() == 0) {
            if (bddisemptymember(f)) {
                return mpz_class(0);
            } else {
                return mpz_class(-1);
            }
        }

        mpz_class value(0);
        if (bddisemptymember(f)) {
            value += mpz_class(1);
            f0 = bdderasenot(f);
        } else {
            f0 = f;
        }

        bddvar var = bddgetvar(f);
        if (s.count(var) >= 1) {
            s.erase(var);
            mpz_class v = getOrderNumberMP(bddgetchild1z(f), s);
            if (v >= 0) {
                value += v;
            } else {
                value = mpz_class(-1);
            }
        } else {
            mpz_class v = count_storage_.at(bddgetchild1z(f))
                + getOrderNumberMP(bddgetchild0z(f0), s);
            if (v >= 0) {
                value += v;
            } else {
                value = mpz_class(-1);
            }
        }
        return value;
    }
#endif

    void getSet(bddp f, llint order, std::set<bddvar>& s)
    {
        bddp f0 = f;

        if (f == bddempty || f == bddsingle) {
            return;
        }

        if (bddisemptymember(f)) {
            if (order == 0) {
                return;
            } else {
                order -= 1;
                f0 = bdderasenot(f);
            }
        }

        llint card1 = getStorageValue(bddgetchild1z(f));
        if (order < card1) {
            s.insert(bddgetvar(f));
            getSet(bddgetchild1z(f), order, s);
        } else {
            getSet(bddgetchild0z(f0), order - card1, s);
        }
    }

#ifdef USE_GMP
    void getSetMP(bddp f, mpz_class order, std::set<bddvar>& s)
    {
        bddp f0 = f;

        if (f == bddempty || f == bddsingle) {
            return;
        }

        if (bddisemptymember(f)) {
            if (order == mpz_class(0)) {
                return;
            } else {
                order -= mpz_class(1);
                f0 = bdderasenot(f);
            }
        }

        mpz_class card1 = count_storage_[bddgetchild1z(f)];
        if (order < card1) {
            s.insert(bddgetvar(f));
            getSetMP(bddgetchild1z(f), order, s);
        } else {
            getSetMP(bddgetchild0z(f0), order - card1, s);
        }
    }
#endif

    bddp getBddp(int level, llint pos) const
    {
        return sbddextended_MyVector_get(&index_->
                                            level_vec_arr[level],
                                            pos);
    }

public:
    DDIndex(const BDD& f, bool is_raw = false)
    {
        initialize(f.GetID(), is_raw, 0);
    }

    DDIndex(const ZBDD& f, bool is_raw = false)
    {
        initialize(f.GetID(), is_raw, 1);
    }

    DDIndex(bddp f, bool is_raw = false)
    {
        initialize(f, is_raw, (bddiszbdd(f) ? 1 : 0));
    }

    ~DDIndex()
    {
        bddNodeIndex_destruct(index_);
        free(index_);
    }

    bddNodeIndex* getRawPointer()
    {
        return index_;
    }

    T& getStorageRef(bddp f)
    {
        return storage_[f];
    }

    int height() const
    {
        return bddgetlev(index_->f);
    }

    llint size() const
    {
        return bddNodeIndex_size(index_);
    }

    llint size(int level) const
    {
        return bddNodeIndex_sizeAtLevel(index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr) const
    {
        if (!(index_->f == bddnull || index_->f == bddfalse || index_->f == bddtrue)) {
            arr.resize(index_->height + 1);
            for (int i = 1; i <= index_->height; ++i) {
                arr[i] = (bddvar)(index_->offset_arr[i - 1] - index_->offset_arr[i]);
            }
        }
    }

    llint count()
    {
        makeCountIndex();
#ifdef USE_GMP
        // only support 32 bit because GMP does not have get_sll
        return static_cast<llint>(count_storage_[index_->f].get_ui());
#else
        return count_storage_[index_->f];
#endif
    }

#ifdef USE_GMP
    mpz_class countMP()
    {
        makeCountIndex();
        return count_storage_[index_->f];
    }
#endif

    llint getMaximum(const std::vector<llint>& weights, std::set<bddvar>& s) const
    {
        return optimize(weights, true, s);
    }

    llint getMaximum(const std::vector<llint>& weights) const
    {
        std::set<bddvar> dummy;
        return optimize(weights, true, dummy);
    }

    llint getMinimum(const std::vector<llint>& weights, std::set<bddvar>& s) const
    {
        return optimize(weights, false, s);
    }

    llint getMinimum(const std::vector<llint>& weights) const
    {
        std::set<bddvar> dummy;
        return optimize(weights, false, dummy);
    }

    llint getSum(const std::vector<llint>& weights)
    {
        if (index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        makeCountIndex();

        std::map<bddp, llint> sto;

        sto[bddempty] = 0;
        sto[bddsingle] = 0;

        for (int level = 1; level <= height(); ++level) {
            for (llint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                sto[f] = sto[child0] + sto[child1] + weights[var] * getStorageValue(child1);
            }
        }
        return sto[index_->f];
    }

#ifdef USE_GMP
    mpz_class getSumMP(const std::vector<llint>& weights)
    {
        if (index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        makeCountIndex();

        std::map<bddp, mpz_class> sto;

        sto[bddempty] = mpz_class(0);
        sto[bddsingle] = mpz_class(0);

        for (int level = 1; level <= height(); ++level) {
            for (llint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                mpz_class w_mp;
                if (weights[var] < (1 << 30)) {
                    w_mp = mpz_class(static_cast<int>(weights[var]));
                } else {
                    std::stringstream ss;
                    ss << weights[var];
                    w_mp = mpz_class(ss.str());
                }
                sto[f] = sto[child0] + sto[child1] + w_mp * count_storage_.at(child1);
            }
        }
        return sto[index_->f];
    }
#endif

    llint getOrderNumber(const std::set<bddvar>& s)
    {
        makeCountIndex();
        std::set<bddvar> ss(s);
        return getOrderNumber(index_->f, ss);
    }

#ifdef USE_GMP
    mpz_class getOrderNumberMP(const std::set<bddvar>& s)
    {
        makeCountIndex();
        std::set<bddvar> ss(s);
        return getOrderNumberMP(index_->f, ss);
    }
#endif

    std::set<bddvar> getSet(llint order)
    {
        makeCountIndex();
        if (order < 0 || order >= count()) { // out of range
            return std::set<bddvar>();
        }
        std::set<bddvar> s;
        getSet(index_->f, order, s);
        return s;
    }

#ifdef USE_GMP
    std::set<bddvar> getSet(mpz_class order)
    {
        makeCountIndex();
        if (order < mpz_class(0) || order >= countMP()) { // out of range
            return std::set<bddvar>();
        }
        std::set<bddvar> s;
        getSetMP(index_->f, order, s);
        return s;
    }
#endif

#ifdef USE_GMP // use GMP random
    std::set<bddvar> sampleRandomly(gmp_randclass& random)
    {
        makeCountIndex();
        return getSet(random.get_z_range(countMP()));
    }
#else // USE_GMP

#if __cplusplus >= 201103L // use C++ random class

    template <typename U>
    std::set<bddvar> sampleRandomly(U& random_engine)
    {
        std::uniform_int_distribution<llint> dist(0, count() - 1);
        return getSet(dist(random_engine));
    }

#else // __cplusplus >= 201103L // use rand() function

    std::set<bddvar> sampleRandomly()
    {
        makeCountIndex();
        if (count() >= 1) {
            return getSet(rand() % count());
        } else {
            return std::set<bddvar>();
        }
    }

#endif // __cplusplus >= 201103L

#endif // USE_GMP

    DDNode<T> root()
    {
        return DDNode<T>(index_->f, *this);
    }

    DDNode<T> terminal(int t)
    {
        return DDNode<T>((t != 0 ? bddtrue : bddfalse), *this);
    }

    DDNode<T> getNode(int level, llint pos)
    {
        return DDNode<T>(getBddp(level, pos), *this);
    }

    void makeCountIndex() // currently support only for ZDD
    {
        if (!is_count_made) {
            is_count_made = true;

            if (index_->is_raw) {
                std::cerr << "DDIndex currently does not support raw mode." << std::endl;
                exit(1);
            }
            count_storage_[bddempty] = sbddextended_VALUE_ZERO;
            count_storage_[bddsingle] = sbddextended_VALUE_ONE;
            for (int level = 1; level <= height(); ++level) {
                for (llint pos = 0; pos < size(level); ++pos) {
                    bddp f = getBddp(level, pos);
                    bddp child0 = bddgetchild0z(f);
                    bddp child1 = bddgetchild1z(f);
                    count_storage_[f] = count_storage_[child0] + count_storage_[child1];
                }
            }
        }
    }

    class DDNodeIterator : public std::iterator<std::input_iterator_tag, bddp>
    {
    private:
        const DDIndex& index_;
        size_t pos_;
        size_t level_;

    public:
        DDNodeIterator(const DDIndex& index, bool is_end) : index_(index), pos_(0)
        {
            if (is_end) {
                level_ = 0; // This means pointing at end;
            } else {
                level_ = index.index_->height;
            }
        }

        bddp operator*() const
        {
            if (level_ <= 0) {
                return bddfalse;
            }
            return sbddextended_MyVector_get(&index_.index_->
                                                level_vec_arr[level_],
                                                (llint)pos_);
        }

        DDNodeIterator& operator++()
        {
            if (level_ > 0) {
                ++pos_;
                while (level_ > 0 &&
                        pos_ >= index_.index_->level_vec_arr[level_].count) {
                    pos_ = 0;
                    --level_;
                }
            }
            return *this;
        }

        bool operator==(const DDNodeIterator& it) const
        {
            if (level_ <= 0) {
                return it.level_ <= 0;
            } else {
                return pos_ == it.pos_ && level_ == it.level_;
            }
        }

        bool operator!=(const DDNodeIterator& it) const
        {
            return !(operator==(it));
        }
    };

    DDNodeIterator begin()
    {
        return DDNodeIterator(*this, false);
    }

    DDNodeIterator end()
    {
        return DDNodeIterator(*this, true);
    }

};

#endif // __cplusplus

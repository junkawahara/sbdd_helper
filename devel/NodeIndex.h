typedef struct tagbddNodeIndex {
    int is_raw;
    int is_zbdd;
    /* All of the following four pointers are NULL if f is a terminal or bddnull. */
    sbddextended_MyDict* node_dict_arr;
    sbddextended_MyVector* level_vec_arr; /* stores all nodes at level i */
    llint* offset_arr;
    ullint* count_arr; /* array representing the number of solutions for node i */
    int height;
    bddp f;
} bddNodeIndex;

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexWithoutCount_inner(bddp f, int is_raw, int is_zbdd)
{
    int i, k, level;
    size_t j;
    bddp node, child;
    bddNodeIndex* node_index;

    node_index = (bddNodeIndex*)malloc(sizeof(bddNodeIndex));
    if (node_index == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node_index->is_raw = (is_raw != 0 ? 1 : 0);
    node_index->f = f;
    node_index->height = (int)bddgetlev(f);
    node_index->is_zbdd = (is_zbdd != 0 ? 1 : 0);

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        node_index->node_dict_arr = NULL;
        node_index->level_vec_arr = NULL;
        node_index->offset_arr = NULL;
        node_index->count_arr = NULL;
        return node_index;
    }

    if (is_raw) {
        f = bdderasenot(f);
    }

    node_index->node_dict_arr = (sbddextended_MyDict*)malloc(
                            (size_t)(node_index->height + 1) * sizeof(sbddextended_MyDict));
    if (node_index->node_dict_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node_index->level_vec_arr = (sbddextended_MyVector*)malloc(
                            (size_t)(node_index->height + 1) * sizeof(sbddextended_MyVector));
    if (node_index->level_vec_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (i = 1; i <= node_index->height; ++i) {
        sbddextended_MyDict_initialize(&node_index->node_dict_arr[i]);
        sbddextended_MyVector_initialize(&node_index->level_vec_arr[i]);
    }

    sbddextended_MyDict_add(&node_index->node_dict_arr[node_index->height],
                            (llint)f,
                            0ll); /* 0 means the first node in the level */
    sbddextended_MyVector_add(&node_index->level_vec_arr[node_index->height], (llint)f);

    for (i = node_index->height; i >= 1; --i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_raw) {
                    if (is_zbdd) {
                        child = bddgetchildzraw(node, k);
                    } else {
                        child = bddgetchildbraw(node, k);
                    }
                    child = bdderasenot(child);
                } else {
                    if (is_zbdd) {
                        child = bddgetchildz(node, k);
                    } else {
                        child = bddgetchildb(node, k);
                    }
                }

                if (!bddisterminal(child)) {
                    level = (int)bddgetlev(child);
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[level],
                                                 (llint)child, NULL) == 0) {
                        sbddextended_MyDict_add(&node_index->node_dict_arr[level],
                                                (llint)child,
                                                (llint)node_index->node_dict_arr[level].count);
                        sbddextended_MyVector_add(&node_index->level_vec_arr[level], (llint)child);
                    }
                }
            }
        }
    }

    node_index->offset_arr = (llint*)malloc((size_t)(node_index->height + 1) * sizeof(llint));
    if (node_index->offset_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node_index->offset_arr[node_index->height] = sbddextended_BDDNODE_START;

    for (i = node_index->height; i >= 1; --i) {
        node_index->offset_arr[i - 1] = node_index->offset_arr[i] + (llint)node_index->level_vec_arr[i].count;
    }

    node_index->count_arr = NULL;
    return node_index;
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
bddNodeIndex* bddNodeIndex_makeIndex_inner(bddp f, int is_raw, int is_zbdd)
{
    int i, clevel, raw_flag;
    llint j, id0, id1;
    bddp node, n0, n1;
    bddNodeIndex* node_index;

    node_index = bddNodeIndex_makeIndexWithoutCount_inner(f, is_raw, is_zbdd);

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        return node_index;
    }

    if (is_zbdd) {
        node_index->count_arr = (ullint*)malloc((size_t)node_index->offset_arr[0] * sizeof(ullint));
        if (node_index->count_arr == NULL) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
        node_index->count_arr[0] = 0;
        node_index->count_arr[1] = 1;

        for (i = 1; i <= node_index->height; ++i) {
            for (j = node_index->offset_arr[i]; j < node_index->offset_arr[i - 1]; ++j) {
                node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i],
                                                       j - node_index->offset_arr[i]);
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
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                     (llint)n0, &id0) == 0) {
                        fprintf(stderr, "node not found!\n");
                        exit(1);
                    }
                    id0 += node_index->offset_arr[clevel];
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
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                     (llint)n1, &id1) == 0) {
                        fprintf(stderr, "node not found!\n");
                        exit(1);
                    }
                    id1 += node_index->offset_arr[clevel];
                }
                /* We do not check the overflow. */
                node_index->count_arr[j] = node_index->count_arr[id0] + node_index->count_arr[id1];
                if (is_raw && raw_flag) {
                    node_index->count_arr[j] += 1;
                }
            }
        }
    } else {
        /* not implemented yet. */
    }
    return node_index;
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
ullint bddNodeIndex_size(const bddNodeIndex* node_index)
{
    if (node_index->f == bddnull || node_index->f == bddfalse || node_index->f == bddtrue) {
        return 0ull;
    }
    return (ullint)node_index->offset_arr[0] - sbddextended_BDDNODE_START;
}

sbddextended_INLINE_FUNC
ullint bddNodeIndex_sizeAtLevel(const bddNodeIndex* node_index, int level)
{
    if (node_index->f == bddnull || node_index->f == bddfalse || node_index->f == bddtrue) {
        return 0ull;
    } else if (level <= 0 || node_index->height < level) {
        return 0ull;
    }
    assert(node_index->offset_arr[level - 1] >= node_index->offset_arr[level]);
    return (ullint)(node_index->offset_arr[level - 1] - node_index->offset_arr[level]);
}

sbddextended_INLINE_FUNC
void bddNodeIndex_sizeEachLevel(const bddNodeIndex* node_index, bddvar* arr)
{
    int i;
    if (!(node_index->f == bddnull || bddisterminal(node_index->f))) {
        for (i = 1; i <= node_index->height; ++i) {
            assert(node_index->offset_arr[i - 1] >= node_index->offset_arr[i]);
            arr[i] = (bddvar)(node_index->offset_arr[i - 1] - node_index->offset_arr[i]);
        }
    }
}

sbddextended_INLINE_FUNC
ullint bddNodeIndex_count(const bddNodeIndex* node_index)
{
    /* We assume that the two values are the same. */
    assert(bddtrue == bddsingle);
    if (node_index->f == bddnull || node_index->f == bddfalse
            || node_index->f == bddempty) {
        return 0ull;
    } else if (node_index->f == bddtrue) {
        return 1ull;
    }
    if (node_index->is_raw) {
        if (bddisnegative(node_index->f)) {
            return node_index->count_arr[sbddextended_BDDNODE_START] + 1;
        } else {
            return node_index->count_arr[sbddextended_BDDNODE_START];
        }
    } else {
        return node_index->count_arr[sbddextended_BDDNODE_START];
    }
}

sbddextended_INLINE_FUNC
void bddNodeIndex_destruct(bddNodeIndex* node_index)
{
    int i;

    if (node_index != NULL) {
        if (node_index->level_vec_arr != NULL) {
            for (i = 1; i <= node_index->height; ++i) {
                sbddextended_MyVector_deinitialize(&node_index->level_vec_arr[i]);
            }
            free(node_index->level_vec_arr);
        }
        if (node_index->node_dict_arr != NULL) {
            for (i = 1; i <= node_index->height; ++i) {
                sbddextended_MyDict_deinitialize(&node_index->node_dict_arr[i]);
            }
            free(node_index->node_dict_arr);
        }
        if (node_index->offset_arr != NULL) {
            free(node_index->offset_arr);
        }
        if (node_index->count_arr != NULL) {
            free(node_index->count_arr);
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


/* *************************** C++ version start ***************************** */

#ifdef __cplusplus

/* This class is obsolate. */
class DDNodeIndex {
private:
    bddNodeIndex* node_index_;

public:
    DDNodeIndex(const BDD& f, bool is_raw = true)
    {
        node_index_ = bddNodeIndex_makeIndex_inner(f.GetID(), (is_raw ? 1 : 0), 0);
    }

    DDNodeIndex(const ZBDD& f, bool is_raw = true)
    {
        node_index_ = bddNodeIndex_makeIndex_inner(f.GetID(), (is_raw ? 1 : 0), 1);
    }

    bddNodeIndex* getRawPointer()
    {
        return node_index_;
    }

    ullint size()
    {
        return bddNodeIndex_size(node_index_);
    }

    ullint sizeAtLevel(int level)
    {
        return bddNodeIndex_sizeAtLevel(node_index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr)
    {
        if (!(node_index_->f == bddnull || node_index_->f == bddfalse || node_index_->f == bddtrue)) {
            arr.resize(node_index_->height + 1);
            for (int i = 1; i <= node_index_->height; ++i) {
                arr[i] = (bddvar)(node_index_->offset_arr[i - 1] - node_index_->offset_arr[i]);
            }
        }
    }

    ullint count()
    {
        return bddNodeIndex_count(node_index_);
    }

    ~DDNodeIndex()
    {
        bddNodeIndex_destruct(node_index_);
        free(node_index_);
    }

#if __cplusplus >= 201703L
    class DDNodeIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = bddp;
        using difference_type = std::ptrdiff_t;
        using pointer = bddp*;
        using reference = bddp&;
    private:
#else
    class DDNodeIterator : public std::iterator<std::input_iterator_tag, bddp>
    {
    private:
#endif
        DDNodeIndex* node_index_;
        size_t pos_;
        size_t level_;

    public:
        DDNodeIterator(DDNodeIndex* node_index, bool is_end) : node_index_(node_index), pos_(0)
        {
            if (is_end) {
                level_ = 0; /* This means pointing at end; */
            } else {
                level_ = node_index->node_index_->height;
            }
        }

        bddp operator*() const
        {
            if (level_ <= 0) {
                return bddfalse;
            }
            return (bddp)sbddextended_MyVector_get(&node_index_->node_index_->
                                                    level_vec_arr[level_],
                                                    (llint)pos_);
        }

        DDNodeIterator& operator++()
        {
            if (level_ > 0) {
                ++pos_;
                while (level_ > 0 &&
                       pos_ >= node_index_->node_index_->level_vec_arr[level_].count) {
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
        return DDNodeIterator(this, false);
    }

    DDNodeIterator end()
    {
        return DDNodeIterator(this, true);
    }
};

template <typename T> class DDIndex;

template <typename T>
class DDNode {
private:
    const bddp f_;
    DDIndex<T>& node_index_;
public:
    T& value;

public:
    DDNode(bddp f, DDIndex<T>& node_index) : f_(f), node_index_(node_index), value(node_index.getStorageRef(f)) { }

    bddp getBddp() const
    {
        return f_;
    }

    DDNode<T> child(int c) const
    {
        bddp g;
        if (isTerminal()) {
            g = f_;
        } else if (bddisbdd(f_)) {
            g = bddgetchildb(f_, c);
        } else {
            g = bddgetchildz(f_, c);
        }
        DDNode<T> node(g, node_index_);
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

template <typename T>
bool operator==(const DDNode<T>& node1, const DDNode<T>& node2)
{
    return node1.getBddp() == node2.getBddp();
}

template <typename T>
bool operator!=(const DDNode<T>& node1, const DDNode<T>& node2)
{
    return node1.getBddp() != node2.getBddp();
}

template <typename T>
class DDIndex {
private:
    bddNodeIndex* node_index_;
    std::map<bddp, T> storage_;
    bool is_count_made;

#ifdef SBDDH_GMP
    typedef mpz_class count_t;
#else
    typedef ullint count_t;
#endif
    typedef std::map<bddp, count_t> map_t;
    map_t count_storage_;

    void initialize(bddp f, bool /*is_raw*/, int is_zbdd)
    {
        /* currently, we do not support raw mode. We set is_raw to be false. */
        /*node_index_ = bddNodeIndex_makeIndexWithoutCount_inner(f, (is_raw ? 1 : 0), is_zbdd); */
        node_index_ = bddNodeIndex_makeIndexWithoutCount_inner(f, 0, is_zbdd);
    }

    llint optimize(const std::vector<llint>& weights, bool is_max,
                    std::set<bddvar>& s) const
    {
        if (node_index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        /* llint -> max/min value, bool -> 1-arc if true, 0-arc if false */
        std::map<bddp, std::pair<llint, bool> > sto;

        if (is_max) {
            sto[bddempty].first = LLONG_MIN;
        } else {
            sto[bddempty].first = LLONG_MAX;
        }
        sto[bddsingle].first = 0;

        for (int level = 1; level <= height(); ++level) {
            for (ullint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                if (is_max) {
                    if (sto[child1].first == sto[bddempty].first
                            || sto[child0].first > sto[child1].first + weights[var]) {
                        sto[f].first = sto[child0].first;
                        sto[f].second = false; /* 0-arc side */
                    } else {
                        sto[f].first = sto[child1].first + weights[var];
                        sto[f].second = true; /* 1-arc side */
                    }
                } else {
                    if (sto[child1].first == sto[bddempty].first
                            || sto[child0].first > sto[child1].first + weights[var]) {
                        sto[f].first = sto[child1].first + weights[var];
                        sto[f].second = true; /* 1-arc side */
                    } else {
                        sto[f].first = sto[child0].first;
                        sto[f].second = false; /* 0-arc side */
                    }
                }
            }
        }
        bddp g = node_index_->f;
        while (!bddisterminal(g)) {
            if (sto[g].second) { /* 1-arc */
                s.insert(bddgetvar(g));
                g = bddgetchild1z(g);
            } else { /* 0-arc */
                g = bddgetchild0z(g);
            }
        }
        assert(g == bddsingle);
        return sto[node_index_->f].first;
    }

#ifdef SBDDH_GMP
    ullint getStorageValue(bddp f) const
    {
        if (count_storage_.count(f) > 0) {
            return sbddh_mpz_to_ullint(count_storage_.at(f));
        } else {
            bddp fn = bddtakenot(f);
            std::map<bddp, mpz_class>::const_iterator itor = count_storage_.find(fn);
            if (itor == count_storage_.end()) {
                std::cerr << "key f not found" << std::endl;
                exit(1);
            } else {
                return sbddh_mpz_to_ullint(itor->second) + (bddisnegative(f) ? 1 : -1);
            }
        }
    }

#else

    ullint getStorageValue(bddp f) const
    {
        if (count_storage_.count(f) > 0) {
            return count_storage_.at(f);
        } else {
            bddp fn = bddtakenot(f);
            std::map<bddp, ullint>::const_iterator itor = count_storage_.find(fn);
            if (itor == count_storage_.end()) {
                std::cerr << "key f not found" << std::endl;
                exit(1);
            } else {
                if (bddisnegative(f)) {
                    return itor->second + 1;
                } else {
                    assert(itor->second > 0);
                    return itor->second - 1;
                }
            }
        }
    }

#endif

    template<typename value_t>
    value_t getStorageValue2(bddp f) const
    {
        if (count_storage_.count(f) > 0) {
            return sbddh_getValueFromMpz<value_t>(count_storage_.at(f));
        } else {
            bddp fn = bddtakenot(f);
            map_t::const_iterator itor = count_storage_.find(fn);
            if (itor == count_storage_.end()) {
                std::cerr << "key f not found" << std::endl;
                exit(1);
            } else {
                if (bddisnegative(f)) {
                    return sbddh_getValueFromMpz<value_t>(itor->second)
                        + sbddh_getOne<value_t>();
                } else {
                    assert(itor->second > 0);
                    return sbddh_getValueFromMpz<value_t>(itor->second)
                        - sbddh_getOne<value_t>();
                }
            }
        }
    }

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

#ifdef SBDDH_GMP
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

        llint card1 = static_cast<llint>(getStorageValue(bddgetchild1z(f)));
        if (order < card1) {
            s.insert(bddgetvar(f));
            getSet(bddgetchild1z(f), order, s);
        } else {
            getSet(bddgetchild0z(f0), order - card1, s);
        }
    }

#ifdef SBDDH_GMP
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

    template<typename value_t>
    bddp getKSetsZBDD(bddp f, value_t k)
    {
        if (k <= sbddh_getZero<value_t>() || f == bddempty) {
            return bddempty;
        } else if (f == bddsingle) {
            return bddsingle;
        } else if (k >= getStorageValue2<value_t>(f)) {
            return bddcopy(f);
        } else {
            bddp fn = f;
            bddp f1 = bddgetchild1z(f);
            bddp g;
            value_t card1 = getStorageValue2<value_t>(f1);
            if (bddisemptymember(f)) {
                if (k == sbddh_getOne<value_t>()) {
                    return bddsingle;
                }
                card1 += sbddh_getOne<value_t>();
                fn = bdderasenot(f);
            }
            if (k > card1) {
                bddp f0 = bddgetchild0z(fn);
                bddp g0 = getKSetsZBDD<value_t>(f0, k - card1);
                if (bddisemptymember(f)) {
                    assert(!bddisnegative(g0));
                    g0 = bddtakenot(g0);
                }
                g = bddmakenodez(bddgetvar(f), g0, f1);
                bddfree(g0);
            } else {
                bddp g1;
                if (bddisemptymember(f)) {
                    g1 = getKSetsZBDD<value_t>(f1, k - sbddh_getOne<value_t>());
                } else {
                    g1 = getKSetsZBDD<value_t>(f1, k);
                }

                if (bddisemptymember(f)) {
                    g = bddmakenodez(bddgetvar(f), bddsingle, g1);
                } else {
                    g = bddmakenodez(bddgetvar(f), bddempty, g1);
                }
                bddfree(g1);
            }
#ifdef SBDDH_GMP
            assert(sbddh_getValueFromMpz<value_t>(
                sbddh_ullint_to_mpz(static_cast<ullint>(bddcard(g)))) == k);
#else
            assert(static_cast<ullint>(bddcard(g)) == k);
#endif
            return g;
        }
    }

#ifdef SBDDH_BDDCT
    template<typename value_t>
    ZBDD getKLightestZBDD(const ZBDD& f, const value_t& k,
        const std::vector<llint>& weights, int strict)
    {
        if (k <= sbddh_getZero<value_t>() || f == ZBDD(0)) {
            return ZBDD(0);
        }
        makeCountIndex();
        /* k is more than or equal to the card of f */
        if (k >= getStorageValue2<value_t>(f.GetID())) {
            return f;
        }
        BDDCT bddct;
        const int lev = getLev(f);
        if (!weightRange_initialize(&bddct, lev, weights)) {
            return ZBDD(-1);
        }

        /* binary search: k is in (left_bound, right_bound) */
        int left_bound = bddct.MinCost(f) - 1;
        int right_bound = bddct.MaxCost(f);
        assert(left_bound < right_bound);
        ZBDD c_zbdd;
        ZBDD left_zbdd(0);
        ZBDD right_zbdd = f;
        value_t c_card;
        value_t left_card = 0;
        while (right_bound - left_bound > 1) {
            int c_bound = left_bound + (right_bound - left_bound) / 2;
            assert(c_bound >= left_bound + 1);
            c_zbdd = bddct.ZBDD_CostLE(f, c_bound);
            c_card = sbddh_getCard<value_t>(c_zbdd);
            if (c_card == k) {
                return c_zbdd;
            } else if (c_card < k) {
                left_bound = c_bound;
                left_zbdd = c_zbdd;
                left_card = c_card;
            } else {
                right_bound = c_bound;
                right_zbdd = c_zbdd;
            }
            assert(left_bound < right_bound);
        }
        assert(left_bound + 1 == right_bound);
        /* assert(bddct.ZBDD_CostLE(f, left_bound) == left_zbdd); */
        /* assert(bddct.ZBDD_CostLE(f, right_bound) == right_zbdd); */
        if (strict < 0) {
            return left_zbdd;
        } else if (strict > 0) {
            return right_zbdd;
        } else {
            ZBDD delta = right_zbdd - left_zbdd;
            DDIndex<int> delta_index(delta);
            return left_zbdd + delta_index.getKSetsZBDD(k - left_card);
        }
    }
#endif /* SBDDH_BDDCT */

    template<typename value_t>
    void sampleRandomlyA(ullint* rand_state, std::set<bddvar>& s)
    {
        bddp f = node_index_->f;
        while (!bddisconstant(f)) {
            bddp f0 = bddgetchild0(f);
            bddp f1 = bddgetchild1(f);
            value_t card0 = getStorageValue2<value_t>(f0);
            value_t card1 = getStorageValue2<value_t>(f1);
            double r = static_cast<double>(sbddextended_getXRand(rand_state) - 1)
                    /* / 0xffffffffffffffffull; */
                    / 1.8446744073709552e+19; /* avoid warning */
            if (r < sbddh_divide<value_t>(card0, card0 + card1)) {
                f = f0;
            } else {
                s.insert(bddgetvar(f));
                f = f1;
            }
        }
        assert(f == bddsingle);
    }

    bddp getBddp(int level, ullint pos) const
    {
        return static_cast<bddp>(sbddextended_MyVector_get(&node_index_->
                                                    level_vec_arr[level],
                                                    static_cast<llint>(pos)));
    }

public:
    DDIndex(const BDD& f, bool is_raw = false) : is_count_made(false)
    {
        initialize(f.GetID(), is_raw, 0);
    }

    DDIndex(const ZBDD& f, bool is_raw = false) : is_count_made(false)
    {
        initialize(f.GetID(), is_raw, 1);
    }

    DDIndex(bddp f, bool is_raw = false) : is_count_made(false)
    {
        initialize(f, is_raw, (bddiszbdd(f) ? 1 : 0));
    }

    ~DDIndex()
    {
        clear();
    }

    void clear()
    {
        if (node_index_ != NULL) {
            bddNodeIndex_destruct(node_index_);
            free(node_index_);
            node_index_ = NULL;
        }
        storage_.clear();
        count_storage_.clear();
        is_count_made = false;
    }

    bool is_valid() const
    {
        return node_index_ != NULL;
    }

    bddNodeIndex* getRawPointer()
    {
        return node_index_;
    }

    T& getStorageRef(bddp f)
    {
        return storage_[f];
    }

    ZBDD getZBDD() const
    {
        if (node_index_ == NULL) {
            return ZBDD(0);
        }
        return ZBDD_ID(bddcopy(node_index_->f));
    }

    int height() const
    {
        if (node_index_ == NULL) {
            return 0;
        }
        return bddgetlev(node_index_->f);
    }

    ullint size() const
    {
        if (node_index_ == NULL) {
            return 0ull;
        }
        return bddNodeIndex_size(node_index_);
    }

    ullint size(int level) const
    {
        if (node_index_ == NULL) {
            return 0ull;
        }
        return bddNodeIndex_sizeAtLevel(node_index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr) const
    {
        if (node_index_ == NULL) {
            return;
        }
        if (!(node_index_->f == bddnull || node_index_->f == bddfalse || node_index_->f == bddtrue)) {
            arr.resize(node_index_->height + 1);
            for (int i = 1; i <= node_index_->height; ++i) {
                arr[i] = (bddvar)(node_index_->offset_arr[i - 1] - node_index_->offset_arr[i]);
            }
        }
    }

    std::set<bddvar> usedVar() const
    {
        std::set<bddvar> result;
        std::vector<bddvar> size_arr;
        sizeEachLevel(size_arr);
        for (int lev = 1; lev <= node_index_->height; ++lev) {
            if (size_arr[lev] > 0) {
                result.insert(bddvaroflev(lev));
            }
        }
        return result;
    }

    ullint count()
    {
        if (node_index_ == NULL) {
            return 0ull;
        }
        makeCountIndex();
        return getStorageValue2<ullint>(node_index_->f);
    }

#ifdef SBDDH_GMP
    mpz_class countMP()
    {
        if (node_index_ == NULL) {
            return mpz_class(0);
        }
        makeCountIndex();
        return getStorageValue2<mpz_class>(node_index_->f);
    }

    mpz_class count_v()
    {
        return countMP();
    }
#else
    ullint count_v()
    {
        return count();
    }
#endif

    llint getMaximum(const std::vector<llint>& weights, std::set<bddvar>& s) const
    {
        if (node_index_ == NULL || node_index_->f == bddempty) {
            return 0ll;
        }
        return optimize(weights, true, s);
    }

    llint getMaximum(const std::vector<llint>& weights) const
    {
        if (node_index_ == NULL || node_index_->f == bddempty) {
            return 0ll;
        }
        std::set<bddvar> dummy;
        return optimize(weights, true, dummy);
    }

    llint getMinimum(const std::vector<llint>& weights, std::set<bddvar>& s) const
    {
        if (node_index_ == NULL || node_index_->f == bddempty) {
            return 0ll;
        }
        return optimize(weights, false, s);
    }

    llint getMinimum(const std::vector<llint>& weights) const
    {
        if (node_index_ == NULL || node_index_->f == bddempty) {
            return 0ll;
        }
        std::set<bddvar> dummy;
        return optimize(weights, false, dummy);
    }

    llint getSum(const std::vector<llint>& weights)
    {
        if (node_index_ == NULL) {
            return 0ll;
        }
        if (node_index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        makeCountIndex();

        std::map<bddp, llint> sto;

        sto[bddempty] = 0;
        sto[bddsingle] = 0;

        for (int level = 1; level <= height(); ++level) {
            for (ullint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                sto[f] = sto[child0] + sto[child1] + weights[var] * getStorageValue(child1);
            }
        }
        return sto[node_index_->f];
    }

#ifdef SBDDH_GMP
    mpz_class getSumMP(const std::vector<llint>& weights)
    {
        if (node_index_ == NULL) {
            return mpz_class(0);
        }
        if (node_index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        makeCountIndex();

        std::map<bddp, mpz_class> sto;

        sto[bddempty] = mpz_class(0);
        sto[bddsingle] = mpz_class(0);

        for (int level = 1; level <= height(); ++level) {
            for (ullint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                mpz_class w_mp = sbddh_llint_to_mpz(weights[var]);
                sto[f] = sto[child0] + sto[child1] + w_mp * count_storage_.at(child1);
            }
        }
        return sto[node_index_->f];
    }
#endif

    llint getOrderNumber(const std::set<bddvar>& s)
    {
        if (node_index_ == NULL) {
            return -1;
        }
        makeCountIndex();
        std::set<bddvar> ss(s);
        return getOrderNumber(node_index_->f, ss);
    }

#ifdef SBDDH_GMP
    mpz_class getOrderNumberMP(const std::set<bddvar>& s)
    {
        if (node_index_ == NULL) {
            return mpz_class(-1);
        }
        makeCountIndex();
        std::set<bddvar> ss(s);
        return getOrderNumberMP(node_index_->f, ss);
    }
#endif

    std::set<bddvar> getSet(llint order)
    {
        if (node_index_ == NULL) {
            return std::set<bddvar>();
        }
        makeCountIndex();
        if (order < 0 || order >= static_cast<llint>(count())) { /* out of range */
            return std::set<bddvar>();
        }
        std::set<bddvar> s;
        getSet(node_index_->f, order, s);
        return s;
    }

#ifdef SBDDH_GMP
    std::set<bddvar> getSet(mpz_class order)
    {
        if (node_index_ == NULL) {
            return std::set<bddvar>();
        }
        makeCountIndex();
        if (order < mpz_class(0) || order >= countMP()) { /* out of range */
            return std::set<bddvar>();
        }
        std::set<bddvar> s;
        getSetMP(node_index_->f, order, s);
        return s;
    }
#endif

    ZBDD getKSetsZBDD(ullint k)
    {
        if (node_index_ == NULL || k <= 0) {
            return ZBDD(0);
        }
        makeCountIndex();
        return ZBDD_ID(getKSetsZBDD<ullint>(node_index_->f, k));
    }

#ifdef SBDDH_GMP
    ZBDD getKSetsZBDD(const mpz_class& k)
    {
        if (node_index_ == NULL || k <= 0) {
            return ZBDD(0);
        }
        makeCountIndex();
        return ZBDD_ID(getKSetsZBDD<mpz_class>(node_index_->f, k));
    }
#endif

#ifdef SBDDH_BDDCT
    ZBDD getKLightestZBDD(ullint k,
        const std::vector<llint>& weights, int strict)
    {
        if (node_index_ == NULL) {
            return ZBDD(0);
        }
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return getKLightestZBDD<ullint>(f, k, weights, strict);
    }

#ifdef SBDDH_GMP
    ZBDD getKLightestZBDD(const mpz_class& k,
        const std::vector<llint>& weights, int strict)
    {
        if (node_index_ == NULL) {
            return ZBDD(0);
        }
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return getKLightestZBDD<mpz_class>(f, k, weights, strict);
    }
#endif /* SBDDH_GMP */

    ZBDD getKHeaviestZBDD(ullint k,
        const std::vector<llint>& weights, int strict)
    {
        if (node_index_ == NULL) {
            return ZBDD(0);
        }
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return f - getKLightestZBDD(count() - k, weights, -strict);
    }

#ifdef SBDDH_GMP
    ZBDD getKHeaviestZBDD(const mpz_class& k,
        const std::vector<llint>& weights, int strict)
    {
        if (node_index_ == NULL) {
            return ZBDD(0);
        }
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return f - getKLightestZBDD(countMP() - k, weights, -strict);
    }
#endif /* SBDDH_GMP */

#endif /* SBDDH_BDDCT */

#ifdef SBDDH_GMP /* use GMP random */
    std::set<bddvar> sampleRandomly(gmp_randclass& random)
    {
        if (node_index_ == NULL) {
            return std::set<bddvar>();
        }
        makeCountIndex();
        return getSet(random.get_z_range(countMP()));
    }
#else /* SBDDH_GMP */

#if __cplusplus >= 201103L /* use C++ random class */

    template <typename U>
    std::set<bddvar> sampleRandomly(U& random_engine)
    {
        if (node_index_ == NULL) {
            return std::set<bddvar>();
        }
        std::uniform_int_distribution<llint> dist(0, count() - 1);
        return getSet(dist(random_engine));
    }

#else /* __cplusplus >= 201103L // use rand() function */

    std::set<bddvar> sampleRandomly()
    {
        if (node_index_ == NULL) {
            return std::set<bddvar>();
        }
        makeCountIndex();
        if (count() >= 1) {
            return getSet(rand() % count());
        } else {
            return std::set<bddvar>();
        }
    }

#endif /* __cplusplus >= 201103L */

#endif /* SBDDH_GMP */

    std::set<bddvar> sampleRandomlyA(ullint* rand_state)
    {
        if (node_index_ == NULL) {
            return std::set<bddvar>();
        }
        makeCountIndex();
        if (count() >= 1) {
            std::set<bddvar> s;
#ifdef SBDDH_GMP
            /* card is larger than or equal to 2^64 */
            if (countMP() >= mpz_class("18446744073709551616")) {
                sampleRandomlyA<mpz_class>(rand_state, s);
                return s;
            }
#endif
            sampleRandomlyA<ullint>(rand_state, s);
            return s;
        } else {
            return std::set<bddvar>();
        }
    }

    DDNode<T> root()
    {
        if (node_index_ == NULL) {
            return DDNode<T>(bddfalse, *this);
        }
        return DDNode<T>(node_index_->f, *this);
    }

    DDNode<T> terminal(int t)
    {
        return DDNode<T>((t != 0 ? bddtrue : bddfalse), *this);
    }

    DDNode<T> getNode(int level, llint pos)
    {
        if (node_index_ == NULL) {
            return DDNode<T>(bddfalse, *this);
        }
        return DDNode<T>(getBddp(level, pos), *this);
    }

    void makeCountIndex() /* currently support only for ZDD */
    {
        if (node_index_ == NULL) {
            return;
        }
        if (!is_count_made) {
            is_count_made = true;

            if (node_index_->is_raw) {
                std::cerr << "DDIndex currently does not support raw mode." << std::endl;
                exit(1);
            }
            count_storage_[bddempty] = sbddextended_VALUE_ZERO;
            count_storage_[bddsingle] = sbddextended_VALUE_ONE;
            for (int level = 1; level <= height(); ++level) {
                for (ullint pos = 0; pos < size(level); ++pos) {
                    bddp f = getBddp(level, pos);
                    bddp child0 = bddgetchild0z(f);
                    bddp child1 = bddgetchild1z(f);
                    /* We do not check the overflow when count_t is ullint */
                    count_storage_[f] = count_storage_[child0] + count_storage_[child1];
                }
            }
        }
    }

#if __cplusplus >= 201703L
    class DDNodeIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = bddp;
        using difference_type = std::ptrdiff_t;
        using pointer = bddp*;
        using reference = bddp&;
    private:
#else
    class DDNodeIterator : public std::iterator<std::input_iterator_tag, bddp>
    {
    private:
#endif
    private:
        DDIndex* node_index_;
        size_t pos_;
        size_t level_;

    public:
        DDNodeIterator(DDIndex* node_index, bool is_end) : node_index_(node_index), pos_(0)
        {
            if (is_end || node_index->node_index_ == NULL) {
                level_ = 0; /* This means pointing at end; */
            } else {
                level_ = node_index->node_index_->height;
            }
        }

        bddp operator*() const
        {
            if (level_ <= 0 || node_index_->node_index_ == NULL) {
                return bddfalse;
            }
            return sbddextended_MyVector_get(&node_index_->node_index_->
                                                level_vec_arr[level_],
                                                (llint)pos_);
        }

        DDNodeIterator& operator++()
        {
            if (level_ > 0 && node_index_->node_index_ != NULL) {
                ++pos_;
                while (level_ > 0 &&
                        pos_ >= node_index_->node_index_->level_vec_arr[level_].count) {
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

#if __cplusplus >= 201703L
    class WeightIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::set<bddvar>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::set<bddvar>*;
        using reference = std::set<bddvar>&;
    private:
#else
    class WeightIterator : public std::iterator<std::input_iterator_tag, std::set<bddvar> >
    {
    private:
#endif
        ZBDD f_;
        std::set<bddvar> current_;
        std::vector<llint> weights_;
        bool is_min_;

        void setCurrent()
        {
            current_.clear();
            DDIndex<int> current_index(f_);
            if (is_min_) {
                current_index.getMinimum(weights_, current_);
            } else {
                current_index.getMaximum(weights_, current_);
            }
        }

    public:
        WeightIterator() : f_(ZBDD(0)), is_min_(false) { }

        WeightIterator(const ZBDD& f,
                const std::vector<llint>& weights,
                bool is_min) :
                    f_(f), weights_(weights), is_min_(is_min)
        {
            setCurrent();
        }

        std::set<bddvar> operator*() const
        {
            return current_;
        }

        WeightIterator& operator++()
        {
            f_ -= getSingleSet(current_);
            setCurrent();
            return *this;
        }

        bool operator==(const WeightIterator& it) const
        {
            return f_ == it.f_;
        }

        bool operator!=(const WeightIterator& it) const
        {
            return !(operator==(it));
        }
    };

#if __cplusplus >= 201703L
    class RandomIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::set<bddvar>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::set<bddvar>*;
        using reference = std::set<bddvar>&;
    private:
#else
    class RandomIterator : public std::iterator<std::input_iterator_tag, std::set<bddvar> >
    {
    private:
#endif
        ZBDD f_;
        std::set<bddvar> current_;
        ullint rand_seed_;

        void setCurrent()
        {
            DDIndex<int> current_index(f_);
            current_ = current_index.sampleRandomlyA(&rand_seed_);
        }

    public:
        RandomIterator() : f_(ZBDD(0)),
                rand_seed_(0) { }

        RandomIterator(const ZBDD& f,
                ullint rand_seed) : f_(f),
                rand_seed_(rand_seed)
        {
            setCurrent();
        }

        std::set<bddvar> operator*() const
        {
            return current_;
        }

        RandomIterator& operator++()
        {
            f_ -= getSingleSet(current_);
            setCurrent();
            return *this;
        }

        bool operator==(const RandomIterator& it) const
        {
            return f_ == it.f_;
        }

        bool operator!=(const RandomIterator& it) const
        {
            return !(operator==(it));
        }
    };

#if __cplusplus >= 201703L
    class DictIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::set<bddvar>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::set<bddvar>*;
        using reference = std::set<bddvar>&;
    private:
#else
    class DictIterator : public std::iterator<std::input_iterator_tag, std::set<bddvar> >
    {
    private:
#endif
        DDIndex<T>* dd_index_;
        count_t card_;
        bool reverse_;
        count_t current_;

    public:
        DictIterator(count_t current) :
            dd_index_(NULL),
            card_(0),
            reverse_(false),
            current_(current)
        { }

        DictIterator(DDIndex<T>* dd_index, bool reverse) :
            dd_index_(dd_index),
            card_(dd_index->count_v()),
            reverse_(reverse),
            current_(reverse ? card_ : 0)
        { }

        std::set<bddvar> operator*() const
        {
            if (reverse_) {
                return dd_index_->getSet(current_ - 1);
            } else {
                return dd_index_->getSet(current_);
            }
        }

        DictIterator& operator++()
        {
            if (reverse_) {
                --current_;
            } else {
                ++current_;
            }
            return *this;
        }

        bool operator==(const DictIterator& it) const
        {
            return current_ == it.current_;
        }

        bool operator!=(const DictIterator& it) const
        {
            return !(operator==(it));
        }
    };

    DDNodeIterator begin() const
    {
        return DDNodeIterator(const_cast<DDIndex*>(this), false);
    }

    DDNodeIterator end() const
    {
        return DDNodeIterator(const_cast<DDIndex*>(this), true);
    }

    WeightIterator weight_min_begin(const std::vector<llint>& weights) const
    {
        if (node_index_ == NULL) {
            return WeightIterator();
        }
        return WeightIterator(ZBDD_ID(bddcopy(node_index_->f)),
            weights, true);
    }

    WeightIterator weight_min_end() const
    {
        return WeightIterator();
    }

    WeightIterator weight_max_begin(const std::vector<llint>& weights) const
    {
        if (node_index_ == NULL) {
            return WeightIterator();
        }
        return WeightIterator(ZBDD_ID(bddcopy(node_index_->f)),
            weights, false);
    }

    WeightIterator weight_max_end() const
    {
        return WeightIterator();
    }

    RandomIterator random_begin(ullint rand_seed = 1) const
    {
        if (node_index_ == NULL) {
            return RandomIterator();
        }
        return RandomIterator(ZBDD_ID(bddcopy(node_index_->f)), rand_seed);
    }

    RandomIterator random_end() const
    {
        return RandomIterator();
    }

    DictIterator dict_begin()
    {
        if (node_index_ == NULL) {
            return DictIterator(0);
        }
        return DictIterator(this, false);
    }

    DictIterator dict_end()
    {
        if (node_index_ == NULL) {
            return DictIterator(0);
        }
        return DictIterator(count_v());
    }

    DictIterator dict_rbegin()
    {
        if (node_index_ == NULL) {
            return DictIterator(0);
        }
        return DictIterator(this, true);
    }

    DictIterator dict_rend()
    {
        return DictIterator(0);
    }
};

#ifdef SBDDH_GMP

template<>
inline
mpz_class sbddh_getCard<mpz_class>(const ZBDD& f)
{
    DDIndex<int> dd_index(f);
    return dd_index.countMP();
}

#endif /* SBDDH_GMP */

template<>
inline
ullint sbddh_getCard<ullint>(const ZBDD& f)
{
    return static_cast<ullint>(f.Card());
}

#endif /* __cplusplus */

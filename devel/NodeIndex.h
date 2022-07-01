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

#endif // __cplusplus

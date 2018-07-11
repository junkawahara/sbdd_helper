
typedef struct tagbddNodeIterator {
    bddNodeIndex* index;
    size_t pos;
    llint level;
} bddNodeIterator;

sbddextended_INLINE_FUNC
bddNodeIterator* bddNodeIterator_make(bddNodeIndex* index)
{
    bddNodeIterator* itor;

    itor = (bddNodeIterator*)malloc(sizeof(bddNodeIterator));
    if (itor == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    itor->index = index;
    itor->pos = 0;
    itor->level = itor->index->height;
    return itor;
}

sbddextended_INLINE_FUNC
int bddNodeIterator_hasNext(const bddNodeIterator* itor)
{
    return (itor->level > 0 ? 1 : 0);
}

sbddextended_INLINE_FUNC
bddp bddNodeIterator_next(bddNodeIterator* itor)
{
    bddp f;

    if (itor->level <= 0) {
        return bddfalse;
    }

    f = (bddp)sbddextended_MyVector_get(&itor->index->level_vec_arr[itor->level],
                                        (llint)itor->pos);
    ++itor->pos;

    while (itor->level > 0 && itor->pos >= itor->index->level_vec_arr[itor->level].count) {
        itor->pos = 0;
        --itor->level;
    }
    return f;
}

// *************************** C++ version start *****************************

#ifdef __cplusplus


#endif // __cplusplus

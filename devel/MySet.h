
typedef struct tagsbddextended_MySet {
#ifdef __cplusplus
    std::set<llint>* se;
#else
    sbddextended_MyDict dict;
#endif
} sbddextended_MySet;

sbddextended_INLINE_FUNC
void sbddextended_MySet_initialize(sbddextended_MySet* d)
{
#ifdef __cplusplus
    d->se = new std::set<llint>();
#else
    sbddextended_MyDict_initialize(&d->dict);
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MySet_deinitialize(sbddextended_MySet* d)
{
#ifdef __cplusplus
    d->se->clear();
    delete d->se;
#else
    sbddextended_MyDict_deinitialize(&d->dict);
#endif
}


sbddextended_INLINE_FUNC
void sbddextended_MySet_add(sbddextended_MySet* d, llint key)
{
#ifdef __cplusplus
    d->se->insert(key);
#else
    /* value is unused */
    sbddextended_MyDict_add(&d->dict, key, 0ll);
#endif
}

/* returned value: 1 -> found, 0 -> not found */
sbddextended_INLINE_FUNC
int sbddextended_MySet_exists(const sbddextended_MySet* d, llint key)
{
#ifdef __cplusplus
    return (d->se->count(key) > 0 ? 1 : 0);
#else
    return sbddextended_MyDict_find(&d->dict, key, NULL);
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MySet_copy(sbddextended_MySet* dest,
                                const sbddextended_MySet* src)
{
#ifdef __cplusplus
    *dest->se = *src->se;
#else
    sbddextended_MyDict_copy(&dest->dict, &src->dict);
#endif
}

sbddextended_INLINE_FUNC
llint sbddextended_MySet_count(const sbddextended_MySet* d)
{
#ifdef __cplusplus
    return (llint)d->se->size();
#else
    return (llint)d->dict.count;
#endif
}

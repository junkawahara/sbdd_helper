/* Internal type. It is not part of the public API.                    */
/* Raw struct assignment (e.g. "u = v;") must not be used because it   */
/* copies only the owning pointer ("se") or the owning tree ("dict"),  */
/* which leads to use-after-free and double free. Use                  */
/* sbddextended_MySet_copy instead.                                    */
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

/* After this function returns, the only functions that may be called  */
/* on "d" are sbddextended_MySet_initialize (to reuse "d") and         */
/* sbddextended_MySet_deinitialize (that is, deinitializing twice is   */
/* safe).                                                              */
sbddextended_INLINE_FUNC
void sbddextended_MySet_deinitialize(sbddextended_MySet* d)
{
#ifdef __cplusplus
    delete d->se;
    d->se = NULL;
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

/* "dest" must be initialized before calling this function. */
/* The current content of "dest" is discarded. */
sbddextended_INLINE_FUNC
void sbddextended_MySet_copy(sbddextended_MySet* dest,
                                const sbddextended_MySet* src)
{
    if (dest == src) {
        return;
    }
#ifdef __cplusplus
    *dest->se = *src->se;
#else
    sbddextended_MyDict_copy(&dest->dict, &src->dict);
#endif
}

/* The returned count always fits in llint because each element */
/* occupies far more than one byte of memory. */
sbddextended_INLINE_FUNC
llint sbddextended_MySet_count(const sbddextended_MySet* d)
{
#ifdef __cplusplus
    return (llint)d->se->size();
#else
    return (llint)d->dict.count;
#endif
}

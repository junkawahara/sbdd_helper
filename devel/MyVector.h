#define sbddextended_MyVector_INITIAL_BUFSIZE 1024

/* Internal type. It is not part of the public API.                    */
/* Raw struct assignment (e.g. "u = v;") must not be used because it   */
/* copies only the owning pointer ("vec" or "buf"), which leads to     */
/* use-after-free and double free. Use sbddextended_MyVector_copy      */
/* instead.                                                            */
typedef struct tagsbddextended_MyVector {
#ifdef __cplusplus
    std::vector<llint>* vec;
#endif
    /* in the C++ version, always vec.size() == count */
    size_t count;
#ifndef __cplusplus
    size_t capacity;
    llint* buf;
#endif
} sbddextended_MyVector;

sbddextended_INLINE_FUNC
void sbddextended_MyVector_initialize(sbddextended_MyVector* v)
{
#ifdef __cplusplus
    v->vec = new std::vector<llint>();
    v->count = 0;
#else
    v->count = 0;
    v->capacity = sbddextended_MyVector_INITIAL_BUFSIZE;
    v->buf = (llint*)malloc(v->capacity * sizeof(llint));
    if (v->buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
#endif
}

/* After this function returns, the only functions that may be called  */
/* on "v" are sbddextended_MyVector_initialize (to reuse "v") and      */
/* sbddextended_MyVector_deinitialize (that is, deinitializing twice   */
/* is safe).                                                           */
sbddextended_INLINE_FUNC
void sbddextended_MyVector_deinitialize(sbddextended_MyVector* v)
{
#ifdef __cplusplus
    delete v->vec;
    v->vec = NULL;
    v->count = 0;
#else
    free(v->buf);
    v->buf = NULL;
    v->count = 0;
    v->capacity = 0;
#endif
}

sbddextended_INLINE_FUNC
llint sbddextended_MyVector_get(const sbddextended_MyVector* v, llint v_index)
{
#ifdef __cplusplus
    assert(0 <= v_index && (size_t)v_index < v->vec->size());
    return (*v->vec)[(size_t)v_index];
#else
    assert(0 <= v_index && (size_t)v_index < v->count);
    return v->buf[v_index];
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_set(sbddextended_MyVector* v,
                                llint v_index, llint value)
{
#ifdef __cplusplus
    assert(0 <= v_index && (size_t)v_index < v->vec->size());
    (*v->vec)[(size_t)v_index] = value;
#else
    assert(0 <= v_index && (size_t)v_index < v->count);
    v->buf[v_index] = value;
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_add(sbddextended_MyVector* v, llint value)
{
#ifdef __cplusplus
    (*v->vec).push_back(value);
    ++v->count;
    assert(v->vec->size() == static_cast<size_t>(v->count));
#else
    if (v->count >= v->capacity) {
        /* guard against the overflow of both "capacity * 2" and */
        /* "capacity * 2 * sizeof(llint)" */
        if (v->capacity > (size_t)-1 / 2 / sizeof(llint)) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
        v->capacity *= 2;
        assert(v->count < v->capacity);
        v->buf = (llint*)realloc(v->buf, v->capacity * sizeof(llint));
        if (v->buf == NULL) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
    }
    v->buf[v->count] = value;
    ++v->count;
#endif
}

/* "dest" must be initialized before calling this function. */
/* The current content of "dest" is discarded. */
sbddextended_INLINE_FUNC
void sbddextended_MyVector_copy(sbddextended_MyVector* dest,
                                const sbddextended_MyVector* src)
{
#ifndef __cplusplus
    llint* buf;
    size_t capacity;
#endif

    if (dest == src) {
        return;
    }
#ifdef __cplusplus
    *dest->vec = *src->vec;
    dest->count = src->count;
#else
    capacity = sbddextended_MyVector_INITIAL_BUFSIZE;
    if (capacity < src->count) {
        capacity = src->count;
    }
    /* "capacity * sizeof(llint)" cannot overflow because "src->buf" */
    /* already holds "src->count" elements */
    buf = (llint*)malloc(capacity * sizeof(llint));
    if (buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    memcpy(buf, src->buf, src->count * sizeof(llint));
    free(dest->buf);
    dest->buf = buf;
    dest->count = src->count;
    dest->capacity = capacity;
#endif
}

/* The vector must not be empty. */
sbddextended_INLINE_FUNC
void sbddextended_MyVector_pop_back(sbddextended_MyVector* v)
{
#ifdef __cplusplus
    assert(!v->vec->empty());
    v->vec->pop_back();
    --v->count;
#else
    assert(v->count > 0);
    --v->count;
#endif
}

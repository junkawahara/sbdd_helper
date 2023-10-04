#define sbddextended_MyVector_INITIAL_BUFSIZE 1024

typedef struct tagsbddextended_MyVector {
#ifdef __cplusplus
    std::vector<llint>* vec;
#endif
    // in the C++ version, always vec.size() == count
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

sbddextended_INLINE_FUNC
void sbddextended_MyVector_deinitialize(sbddextended_MyVector* v)
{
#ifdef __cplusplus
    v->vec->clear();
    delete v->vec;
    v->count = 0;
#else
    v->capacity = 0;
    free(v->buf);
    v->buf = NULL;
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

sbddextended_INLINE_FUNC
void sbddextended_MyVector_copy(sbddextended_MyVector* dest,
                                const sbddextended_MyVector* src)
{
#ifdef __cplusplus
    *dest->vec = *src->vec;
    dest->count = src->count;
#else
    dest->count = src->count;
    dest->capacity = src->capacity;
    dest->buf = (llint*)malloc(dest->capacity * sizeof(llint));
    if (dest->buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    memcpy(dest->buf, src->buf, dest->count * sizeof(llint));
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_pop_back(sbddextended_MyVector* v)
{
#ifdef __cplusplus
    v->vec->pop_back();
    --v->count;
#else
    --v->count;
#endif
}

#define sbddextended_MyVector_INITIAL_BUFSIZE 1024

typedef struct tagsbddextended_MyVector {
    size_t count;
    size_t capacity;
    llint* buf;
} sbddextended_MyVector;

sbddextended_INLINE_FUNC
void sbddextended_MyVector_initialize(sbddextended_MyVector* v)
{
    v->count = 0;
    v->capacity = sbddextended_MyVector_INITIAL_BUFSIZE;
    v->buf = (llint*)malloc(v->capacity * sizeof(llint));
    if (v->buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_deinitialize(sbddextended_MyVector* v)
{
    v->capacity = 0;
    free(v->buf);
    v->buf = NULL;
}

sbddextended_INLINE_FUNC
llint sbddextended_MyVector_get(const sbddextended_MyVector* v, llint index)
{
    assert(0 <= index && (size_t)index < v->count);
    return v->buf[index];
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_set(sbddextended_MyVector* v,
                               llint index, llint value)
{
    assert(0 <= index && (size_t)index < v->count);
    v->buf[index] = value;
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_add(sbddextended_MyVector* v, llint value)
{
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
}

sbddextended_INLINE_FUNC
void sbddextended_MyVector_copy(sbddextended_MyVector* dest,
                                const sbddextended_MyVector* src)
{
    dest->count = src->count;
    dest->capacity = src->capacity;
    dest->buf = (llint*)malloc(dest->capacity * sizeof(llint));
    if (dest->buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    memcpy(dest->buf, src->buf, dest->count * sizeof(llint));
}

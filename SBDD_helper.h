#ifndef SBDD_HELPER_H
#define SBDD_HELPER_H

// One header library for SAPPOROBDD C/C++ version

#ifdef __cplusplus

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <string>
#include <iostream>
#include <fstream>

#else

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#endif

// inline function qualifier for gcc
// if a compile error occurs, change the qualifier
#define sbddextended_INLINE_FUNC static inline

#define sbddextended_MAX_LINE 256
#define sbddextended_BDDNODE_START 2

#define sbddextended_MyVector_INITIAL_BUFSIZE 1024

typedef struct tagsbddextended_MyVector {
    int64_t count;
    int64_t capacity;
    int64_t* buf;
} sbddextended_MyVector;

sbddextended_INLINE_FUNC
void sbddextended_MyVector_initialize(sbddextended_MyVector* v)
{
    v->count = 0;
    v->capacity = sbddextended_MyVector_INITIAL_BUFSIZE;
    v->buf = (int64_t*)malloc(v->capacity * sizeof(int64_t));
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
int64_t sbddextended_MyVector_get(sbddextended_MyVector* v, int64_t index)
{
    assert(0 <= index && index < v->count);
    return v->buf[index];
}

sbddextended_INLINE_FUNC
int64_t sbddextended_MyVector_set(sbddextended_MyVector* v,
                                  int64_t index, int64_t value)
{
    assert(0 <= index && index < v->count);
    v->buf[index] = value;
}

sbddextended_INLINE_FUNC
int64_t sbddextended_MyVector_add(sbddextended_MyVector* v, int64_t value)
{
    if (v->count >= v->capacity) {
        v->capacity *= 2;
        assert(0 <= v->count && v->count < v->capacity);
        v->buf = (int64_t*)malloc(v->capacity * sizeof(int64_t));
        if (v->buf == NULL) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
    }
    v->buf[v->count] = value;
    ++v->count;
}

sbddextended_INLINE_FUNC 
int sbddextended_getLine_inner(char* buf, FILE* fp)
{
    int n;
    char* p;
    p = fgets(buf, sbddextended_MAX_LINE, fp);
    if (p == NULL) {
        return 0;
    }
    n = strlen(buf);
    if (buf[n - 1] != '\n') {
        fprintf(stderr, "Each line must not exceed %d characters\n",
                sbddextended_MAX_LINE);
        exit(1);
    }
    return 1;
}

#ifdef __cplusplus

class GetLineObject {
private:
    bool is_fstream_;
    std::istream* ist_;

public:
    GetLineObject(bool is_fstream, std::istream* ist)
        : is_fstream_(is_fstream), ist_(ist) {}

    bool operator()(char* buf, FILE* fp) const {
        if (is_fstream_) {
            std::string str;
            if (!std::getline(*ist_, str)) {
                return false;
            }
            if (str.length() >= sbddextended_MAX_LINE - 1) {
                fprintf(stderr, "Each line must not exceed %d characters\n",
                    sbddextended_MAX_LINE);
                exit(1);
            }
            strcpy(buf, str.c_str());
            return true;
        } else {
            return sbddextended_getLine_inner(buf, fp) != 0;
        }
    }
};

#else

sbddextended_INLINE_FUNC 
int sbddextended_getLine(char* buf, FILE* fp)
{
    return sbddextended_getLine_inner(buf, fp);
}

#endif

sbddextended_INLINE_FUNC bddvar bddgetvar_nc(bddp f)
{
    return bddtop(f);
}

sbddextended_INLINE_FUNC bddvar bddgetlev_nc(bddp f)
{
    return bddlevofvar(bddtop(f));
}

sbddextended_INLINE_FUNC bddp bddgetchild0z_nc(bddp f)
{
    bddp g;
    g = bddoffset(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild1z_nc(bddp f)
{
    bddp g;
    g = bddonset0(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC
void bddprintzbddelements_printelement(FILE* fp, bddp* vec, char* op, int n,
                                       const char* delim1,
                                       const char* delim2, int* is_first)
{
    int i;
    if (*is_first) {
        *is_first = 0;
    } else {
        fprintf(fp, "%s", delim1);
    }
    for (i = 0; i < n; ++i) {
        if (op[i] == 0) {
            fprintf(fp, "%d", bddgetvar_nc(vec[i]));
            if (i < n - 1) {
                fprintf(fp, "%s", delim2);
            }
        }
    }
}

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1,
                          const char* delim2)
{
    bddp* bddnode_stack;
    char* op_stack;
    char op;
    int height, sp, is_first;
    bddp node, child;

    if (f == bddnull) {
        fprintf(fp, "N");
        return;
    }

    if (f == bddempty) {
        fprintf(fp, "F");
        return;
    } else if (f == bddsingle) {
        fprintf(fp, "T");
        return;
    }

    height = (int)bddgetlev_nc(f) + 1 + 100;
    bddnode_stack = (bddp*)malloc(height * sizeof(bddp));
    if (bddnode_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc(height * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    is_first = 1;
    sp = 0;
    bddnode_stack[sp] = f;
    op_stack[sp] = 0;

    while (sp >= 0) {
        node = bddnode_stack[sp];
        op = op_stack[sp];
        if (node == bddempty) {
            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        } else if (node == bddsingle) {
            bddprintzbddelements_printelement(fp, bddnode_stack, op_stack,
                                              sp,
                                              delim1, delim2, &is_first);
            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        } else {
            if (op <= 1) {
                if (op == 0) {
                    child = bddgetchild1z_nc(node);
                } else {
                    child = bddgetchild0z_nc(node);
                }
                ++sp;
                bddnode_stack[sp] = child;
                op_stack[sp] = 0;
            } else {
                --sp;
                if (sp < 0) {
                    break;
                }
                ++op_stack[sp];
            }
        }
    }
    free(op_stack);
    free(bddnode_stack);
}

sbddextended_INLINE_FUNC
bddp bddconstructzddfromfileknuth_inner(FILE* fp, int is_hex
#ifdef __cplusplus
                             , const GetLineObject& sbddextended_getLine
#endif
                             )
{
    int c, n, level, level_count = 1;
    int64_t i, id, lo, hi, line_count = 0;
    char buf[sbddextended_MAX_LINE];
    bddp p, p0, p1;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec, lo_vec, hi_vec;

    sbddextended_MyVector_initialize(&level_vec);
    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    sbddextended_MyVector_add(&lo_vec, 0);
    sbddextended_MyVector_add(&lo_vec, 1);
    sbddextended_MyVector_add(&hi_vec, 0);
    sbddextended_MyVector_add(&hi_vec, 1);

    while (sbddextended_getLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                exit(1);
            }
            assert(level == level_count);
            ++level_count;
            sbddextended_MyVector_add(&level_vec, lo_vec.count);
            break;
        }
    }

    while (sbddextended_getLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n");
                exit(1);
            }
            assert(level == level_count);
            ++level_count;
            sbddextended_MyVector_add(&level_vec, lo_vec.count);
        } else {
            if (is_hex) {
                c = sscanf(buf, "%llx:%llx,%llx", &id, &lo, &hi);
            } else {
                c = sscanf(buf, "%lld:%lld,%lld", &id, &lo, &hi);
            }
            if (c < 3) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                exit(1);
            }
            if (id != lo_vec.count) {
                fprintf(stderr, "Format error in line %lld %d\n",
                        line_count);
                exit(1);
            }
            sbddextended_MyVector_add(&lo_vec, lo);
            sbddextended_MyVector_add(&hi_vec, hi);
        }
    }
    sbddextended_MyVector_add(&level_vec, lo_vec.count);

    while (bddvarused() < level_vec.count - 1) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc(lo_vec.count * sizeof(bddp));
    bddnode_buf[0] = bddempty;
    bddnode_buf[1] = bddsingle;

    for (i = lo_vec.count - 1; i >= sbddextended_BDDNODE_START; --i) {
        for (level = 1; level < level_vec.count; ++level) {
            if (sbddextended_MyVector_get(&level_vec, level - 1) <= i &&
                i < sbddextended_MyVector_get(&level_vec, level)) {
                break;
            }
        }
        assert(level < level_vec.count);
        p0 = bddnode_buf[sbddextended_MyVector_get(&lo_vec, i)];
        p1 = bddchange(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)],
                       level);
        bddnode_buf[i] = bddunion(p0, p1);
        bddfree(p1);
    }
    for (i = lo_vec.count - 1;
         i >= sbddextended_BDDNODE_START + 1; --i) {
        bddfree(bddnode_buf[i]);
    }

    p = bddnode_buf[sbddextended_BDDNODE_START];

    free(bddnode_buf);

    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);

    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD ConstructZDDFromFileKnuth(FILE* fp, bool is_hex)
{
    GetLineObject glo(false, NULL);
    bddp p;
    p = bddconstructzddfromfileknuth_inner(fp, (is_hex ? 1 : 0), glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD ConstructZDDFromFileKnuth(std::istream& ist, bool is_hex)
{
    GetLineObject glo(true, &ist);
    bddp p;
    p = bddconstructzddfromfileknuth_inner(NULL, (is_hex ? 1 : 0), glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzddfromfileknuth(FILE* fp, int is_hex)
{
    GetLineObject glo(false, NULL);
    return bddconstructzddfromfileknuth_inner(fp, (is_hex ? 1 : 0), glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzddfromfileknuth(FILE* fp, int is_hex)
{
    return bddconstructzddfromfileknuth_inner(fp, is_hex);
}

#endif

#endif // SBDD_HELPER_H

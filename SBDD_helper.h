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
#define sbddextended_NUMBER_OF_CHILDREN 2

#define sbddextended_TEMP_BUFSIZE 1024

//******************** MyVector start ********************

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

//******************** MyVector end ********************

//******************** MyDict start ********************

typedef struct tagsbddextended_MyDictNode {
    struct tagsbddextended_MyDictNode* left;
    struct tagsbddextended_MyDictNode* right;
    int64_t key;
    int64_t value;
} sbddextended_MyDictNode;

sbddextended_INLINE_FUNC
sbddextended_MyDictNode* sbddextended_MyDictNode_MakeNewNode(int64_t key,
                                                            int64_t value)
{
    sbddextended_MyDictNode* node;

    node = (sbddextended_MyDictNode*)malloc(sizeof(sbddextended_MyDictNode));
    if (node == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node->left = NULL;
    node->right = NULL;
    node->key = key;
    node->value = value;
    return node;
}

typedef struct tagsbddextended_MyDict {
    int64_t count;
    sbddextended_MyDictNode* root;
} sbddextended_MyDict;

sbddextended_INLINE_FUNC
void sbddextended_MyDict_initialize(sbddextended_MyDict* d)
{
    d->count = 0;
    d->root = NULL;
}

sbddextended_INLINE_FUNC
void sbddextended_MyDict_deinitialize(sbddextended_MyDict* d)
{
    sbddextended_MyDictNode** node_stack;
    char* op_stack;
    char op;
    int sp;
    sbddextended_MyDictNode* node;
    sbddextended_MyDictNode* child;
    int stack_size;
    int64_t debug_count;

    assert((debug_count = 0) || 1);

    stack_size = d->count + 1;

    node_stack = (sbddextended_MyDictNode**)malloc(stack_size * sizeof(sbddextended_MyDictNode*));
    if (node_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc(stack_size * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    sp = 0;
    node_stack[sp] = d->root;
    op_stack[sp] = 0;

    //fprintf(stderr, "stack_size = %d\n", stack_size);

    // free each node recursively (not using a recursive function)
    while (sp >= 0) {
        //fprintf(stderr, "sp = %d, ", sp);
        node = node_stack[sp];
        op = op_stack[sp];
        //fprintf(stderr, "op = %d, node = %p, left = %p, right = %p\n", op, node, node->left, node->right);

        if (node == NULL) {
            op = 2;
        }

        while (op <= 1) {
            if (op == 0) {
                child = node->left;
            } else { // op == 1
                child = node->right;
            }
            if (child == NULL) {
                ++op;
                ++op_stack[sp];
            } else {
                break;
            }
        }
        if (op <= 1) {
            ++sp;
            node_stack[sp] = child;
            op_stack[sp] = 0;
        } else {
            assert((++debug_count) || 1);
            free(node_stack[sp]);
            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        }
    }
    assert(debug_count == d->count);
    d->count = 0;
    d->root = NULL;
}


sbddextended_INLINE_FUNC
void sbddextended_MyDict_add(sbddextended_MyDict* d, int64_t key, int64_t value)
{
    sbddextended_MyDictNode* node;
    if (d->root == NULL) {
        d->root = sbddextended_MyDictNode_MakeNewNode(key, value);
    } else {
        node = d->root;
        while (node != NULL) {
            if (node->key == key) { // found
                node->value = value;
                break;
            } else if (key < node->key) {
                if (node->left != NULL) {
                    node = node->left;
                } else {
                    node->left = sbddextended_MyDictNode_MakeNewNode(key,
                                                                     value);
                    break;
                }
            } else { // key > node->key
                if (node->right != NULL) {
                    node = node->right;
                } else {
                    node->right = sbddextended_MyDictNode_MakeNewNode(key,
                                                                      value);
                    break;
                }
            }
        }
    }
    ++d->count;
}

// returned value: 1 -> found, 0 -> not found
// The found value is stored into "value" argument.
sbddextended_INLINE_FUNC
int sbddextended_MyDict_find(sbddextended_MyDict* d, int64_t key, int64_t* value)
{
    sbddextended_MyDictNode* node;
    node = d->root;
    while (node != NULL) {
        if (node->key == key) {
            if (value != NULL) {
                *value = node->value;
            }
            return 1;
        } else if (key < node->key) {
            node = node->left;
        } else {// key > node->key
            node = node->right;
        }
    }
    return 0;
}

//******************** MyDict end ********************

//******************** readLine start ********************

sbddextended_INLINE_FUNC 
int sbddextended_readLine_inner(char* buf, FILE* fp)
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

class ReadLineObject {
private:
    bool is_fstream_;
    std::istream* ist_;

public:
    ReadLineObject(bool is_fstream, std::istream* ist)
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
            return sbddextended_readLine_inner(buf, fp) != 0;
        }
    }
};

#else

sbddextended_INLINE_FUNC 
int sbddextended_readLine(char* buf, FILE* fp)
{
    return sbddextended_readLine_inner(buf, fp);
}

#endif

//******************** readLine end ********************

//******************** writeLine start ********************

sbddextended_INLINE_FUNC 
int sbddextended_writeLine_inner(const char* buf, FILE* fp)
{
    int n;
    char* p;

    if (fputs(buf, fp) == EOF || fputc('\n', fp) == EOF) {
        return 0;
        //fprintf(stderr, "Write error!\n",
        //        sbddextended_MAX_LINE);
        //exit(1);
    }
    return 1;
}

#ifdef __cplusplus

class WriteLineObject {
private:
    bool is_fstream_;
    std::ostream* ost_;

public:
    WriteLineObject(bool is_fstream, std::ostream* ost)
        : is_fstream_(is_fstream), ost_(ost) {}

    bool operator()(const char* buf, FILE* fp) const {
        if (is_fstream_) {
            if (!*ost_ || !(*ost_ << buf)) {
                return false;
            }
            if (!*ost_ || !(*ost_ << '\n')) {
                return false;
            }
        } else {
            return sbddextended_writeLine_inner(buf, fp) != 0;
        }
    }
};

#else

sbddextended_INLINE_FUNC 
int sbddextended_writeLine(char* buf, FILE* fp)
{
    return sbddextended_writeLine_inner(buf, fp);
}

#endif

//******************** writeLine end ********************

//******************** SBDD helper start ********************

#define bddisnegative(f) ((f) & B_INV_MASK)
#define bddisconstant(f) ((f) & B_CST_MASK)
#define takenot(f)       ((f) ^ B_INV_MASK)

sbddextended_INLINE_FUNC int bddis64bitversion()
{
#ifdef B_64
    return 1;
#else
    return 0;
#endif
}

sbddextended_INLINE_FUNC int bddisvalid(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC int bddisbdd_nc(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC int bddiszbdd_nc(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC int bddisterminal(bddp f)
{
    return (f == bddempty || f == bddsingle || f == bddfalse || f == bddtrue);
}

sbddextended_INLINE_FUNC bddvar bddgetvar_nc(bddp f)
{
    return bddtop(f);
}

sbddextended_INLINE_FUNC bddvar bddgetlev_nc(bddp f)
{
    return bddlevofvar(bddtop(f));
}

sbddextended_INLINE_FUNC bddp bddgetchild0b_nc(bddp f)
{
    bddp g;
    g = bddat0(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild0z_nc(bddp f)
{
    bddp g;
    g = bddoffset(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild0_nc(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC bddp bddgetchild0braw_nc(bddp f)
{
    if (bddisnegative(f)) {
        return takenot(bddgetchild0b_nc(f));
    } else {
        return bddgetchild0b_nc(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild0zraw_nc(bddp f)
{
    if (bddisnegative(f)) {
        return takenot(bddgetchild0z_nc(f));
    } else {
        return bddgetchild0z_nc(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild0raw_nc(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC bddp bddgetchild1b_nc(bddp f)
{
    bddp g;
    g = bddat1(f, bddtop(f));
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

sbddextended_INLINE_FUNC bddp bddgetchild1_nc(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}
    
sbddextended_INLINE_FUNC bddp bddgetchild1braw_nc(bddp f)
{
    if (bddisnegative(f)) {
        return takenot(bddgetchild1b_nc(f));
    } else {
        return bddgetchild1b_nc(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild1zraw_nc(bddp f)
{
    return bddgetchild1z_nc(f);
}

sbddextended_INLINE_FUNC bddp bddgetchild1raw_nc(bddp f)
{
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC bddp bddgetchildb_nc(bddp f, int child)
{
    return (child != 0 ? bddgetchild1b_nc(f) : bddgetchild0b_nc(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildz_nc(bddp f, int child)
{
    return (child != 0 ? bddgetchild1z_nc(f) : bddgetchild0z_nc(f));
}

sbddextended_INLINE_FUNC bddp bddgetchild_nc(bddp f, int child)
{
    return (child != 0 ? bddgetchild1_nc(f) : bddgetchild0_nc(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildbraw_nc(bddp f, int child)
{
    return (child != 0 ? bddgetchild1braw_nc(f) : bddgetchild0braw_nc(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildzraw_nc(bddp f, int child)
{
    return (child != 0 ? bddgetchild1zraw_nc(f) : bddgetchild0zraw_nc(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildraw_nc(bddp f, int child)
{
    return (child != 0 ? bddgetchild1raw_nc(f) : bddgetchild0raw_nc(f));
}

sbddextended_INLINE_FUNC
bddp bddgetsingleton_nc(bddvar v)
{
    if (v > bddvarused()) {
        fprintf(stderr, "bddgetsingleton: Invalid VarID", v);
        exit(1);
    }
    return bddchange(bddsingle, v);
}

sbddextended_INLINE_FUNC
bddp bddgetsingleset(bddvar* levarr, int n)
{
    int i;
    bddp f, g;

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        assert(1 <= levarr[i] && levarr[i] <= bddvarused());
        g = bddchange(f, levarr[i]);
        bddfree(f);
        f = g;
    }
    return f;
}

sbddextended_INLINE_FUNC
bddp bddgetpowerset(bddvar* levarr, int n)
{
    int i, j;
    bddp f, g, h;
    bddvar* ar;
    bddvar temp;

    ar = (bddvar*)malloc(n * sizeof(bddvar));
    memcpy(ar, levarr, n * sizeof(bddvar));

    for (i = n - 1; i >= 1; --i) {
        for (j = 0; j < i; ++j) {
            if (ar[j] > ar[j + 1]) {
                temp = ar[j];
                ar[j] = ar[j + 1];
                ar[j + 1] = temp;
            }
        }
    }

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        assert(1 <= ar[i] && ar[i] <= bddvarused());
        g = bddchange(f, ar[i]);
        h = bddunion(f, g);
        bddfree(g);
        bddfree(f);
        f = h;
    }
    free(ar);
    return f;
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
bddp bddconstructzddfromfileknuth_inner(FILE* fp, int is_hex, int root_level
#ifdef __cplusplus
                             , const ReadLineObject& sbddextended_readLine
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

    while (sbddextended_readLine(buf, fp)) {
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

    while (sbddextended_readLine(buf, fp)) {
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

    if (root_level < 0) {
        root_level = level_count - 1;
    } else if (root_level < level_count - 1) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZDD.\n");
        exit(1);
    }

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
        assert((1 <= root_level - level + 1) && ((root_level - level + 1) <= bddvarused()));
        p1 = bddchange(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)],
                       bddvaroflev(root_level - level + 1));
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
ZBDD ConstructZDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    bddp p;
    p = bddconstructzddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD ConstructZDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(true, &ist);
    bddp p;
    p = bddconstructzddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    return bddconstructzddfromfileknuth_inner(fp, is_hex,
                                              root_level, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzddfromfileknuth(FILE* fp, int is_hex, int root_level)
{
    return bddconstructzddfromfileknuth_inner(fp, is_hex, root_level);
}

#endif

sbddextended_INLINE_FUNC
void bddwritezddtofileknuth_inner(bddp f, FILE* fp, int is_hex
#ifdef __cplusplus
                          , const WriteLineObject& sbddextended_writeLine
#endif
                             )
{
    int height, level, clevel, i;
    bddp node, child, n0, n1;
    int64_t id, id0, id1, k, j;
    sbddextended_MyDict* visited_node_dict;
    sbddextended_MyVector* level_vec;
    int64_t* offset;
    char ss[sbddextended_TEMP_BUFSIZE];

    if (f == bddnull) {
        return;
    }

    if (f == bddempty) {
        sbddextended_writeLine("0", fp);
        return;
    } else if (f == bddsingle) {
        sbddextended_writeLine("1", fp);
        return;
    }

    height = (int)bddgetlev_nc(f);

    visited_node_dict = (sbddextended_MyDict*)malloc(
                            (height + 1) * sizeof(sbddextended_MyDict));
    if (visited_node_dict == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    level_vec = (sbddextended_MyVector*)malloc(
                    (height + 1) * sizeof(sbddextended_MyVector));
    if (level_vec == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    for (i = 1; i <= height; ++i) {
        sbddextended_MyDict_initialize(&visited_node_dict[i]);
        sbddextended_MyVector_initialize(&level_vec[i]);
    }

    sbddextended_MyDict_add(&visited_node_dict[height],
                            (int64_t)f,
                            0); // 0 means the first node in the level
    sbddextended_MyVector_add(&level_vec[height], (int64_t)f);

    for (i = height; i >= 1; --i) {
        for (j = 0; j < level_vec[i].count; ++j) {
            node = sbddextended_MyVector_get(&level_vec[i], j);
            //fprintf(stderr, "node = %lld\n", node);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (k == 0) {
                    child = bddgetchild0z_nc(node);
                } else {
                    child = bddgetchild1z_nc(node);
                }

                // fprintf(stderr, "child node = %lld\n", child);

                if (!bddisterminal(child)) {
                    //fprintf(stderr, "non terminal");
                    level = bddgetlev_nc(child);
                    //fprintf(stderr, "level = %d\n", level);
                    if (sbddextended_MyDict_find(&visited_node_dict[level],
                                                 (int64_t)child, NULL) == 0) {
                        //fprintf(stderr, "added = %lld\n", child);
                        sbddextended_MyDict_add(&visited_node_dict[level],
                                                (int64_t)child,
                                                visited_node_dict[level].count);
                        sbddextended_MyVector_add(&level_vec[level], (int64_t)child);
                    }
                }
            }
        }
    }

    offset = (int64_t*)malloc((height + 1) * sizeof(int64_t));
    if (offset == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    offset[height] = sbddextended_BDDNODE_START;

    for (i = height; i > 1; --i) {
        offset[i - 1] = offset[i] + level_vec[i].count;
    }

    for (i = height; i >= 1; --i) {
        sprintf(ss, "#%d", height - i + 1);
        sbddextended_writeLine(ss, fp);
        //printf("%s\n", ss);
        for (k = 0; k < level_vec[i].count; ++k) {
            node = (bddp)sbddextended_MyVector_get(&level_vec[i], k);
            n0 = bddgetchild0z_nc(node);
            if (n0 == bddempty) {
                id0 = 0;
            } else if (n0 == bddsingle) {
                id0 = 1;
            } else {
                clevel = (int)bddgetlev_nc(n0);
                if (sbddextended_MyDict_find(&visited_node_dict[clevel],
                                             (int64_t)n0, &id0) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                //fprintf(stderr, "id0 = %lld\n", id0);
                id0 += offset[clevel];
            }
            n1 = bddgetchild1z_nc(node);
            if (n1 == bddempty) {
                id1 = 0;
            } else if (n1 == bddsingle) {
                id1 = 1;
            } else {
                clevel = (int)bddgetlev_nc(n1);
                if (sbddextended_MyDict_find(&visited_node_dict[clevel],
                                             (int64_t)n1, &id1) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                //fprintf(stderr, "id1 = %lld\n", id1);
                id1 += offset[clevel];
            }
            //fprintf(stderr, "node = %lld, %lld, %lld\n", node, n0, n1);
            sprintf(ss, "%lld:%lld,%lld", offset[i] + k, id0, id1);
            sbddextended_writeLine(ss, fp);
            //printf("%s\n", ss);
        }
    }

    for (i = 1; i <= height; ++i) {
        sbddextended_MyVector_deinitialize(&level_vec[i]);
        sbddextended_MyDict_deinitialize(&visited_node_dict[i]);
    }
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void WriteZDDToFileKnuth(ZBDD zbdd, FILE* fp, bool is_hex)
{
    WriteLineObject wlo(false, NULL);
    bddwritezddtofileknuth_inner(zbdd.GetID(), fp, (is_hex ? 1 : 0), wlo);
}

sbddextended_INLINE_FUNC
void WriteZDDToFileKnuth(ZBDD zbdd, std::ostream& ost, bool is_hex)
{
    WriteLineObject wlo(true, &ost);
    bddwritezddtofileknuth_inner(zbdd.GetID(), NULL, (is_hex ? 1 : 0), wlo);
}

sbddextended_INLINE_FUNC
void bddwritezddtofileknuth(bddp f, FILE* fp, int is_hex)
{
    WriteLineObject wlo(false, NULL);
    bddwritezddtofileknuth_inner(f, fp, (is_hex ? 1 : 0), wlo);
}

#else

sbddextended_INLINE_FUNC
void bddwritezddtofileknuth(bddp f, FILE* fp, int is_hex)
{
    bddwritezddtofileknuth_inner(f, fp, is_hex);
}

#endif

#endif // SBDD_HELPER_H

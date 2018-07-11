//
// One header library for SAPPOROBDD C/C++ version
// version 0.02 alpha
//
// Copyright (c) 2017 Jun Kawahara
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SBDD_HELPER_H
#define SBDD_HELPER_H

#ifdef __cplusplus

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdarg>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#endif

#ifdef __cplusplus
namespace sbddh {
#endif

typedef long long int llint;
typedef unsigned long long int ullint;

#define unused(a) (void)(a)

// inline function qualifier for gcc
// if a compile error occurs, change the qualifier
#define sbddextended_INLINE_FUNC static inline

#define sbddextended_MAX_LINE 256
#define sbddextended_BDDNODE_START 2
#define sbddextended_NUMBER_OF_CHILDREN 2

#define sbddextended_TEMP_BUFSIZE 1024


// bddisbdd/bddiszbdd is not supported until version 185 of SAPPOROBDD.
// It may be impossible to implement them unless touching the inner
// structure of SAPPOROBDD.
#if SBDD_VERSION < 185

sbddextended_INLINE_FUNC
int bddisbdd(bddp f)
{
    unused(f);
    fprintf(stderr, "bddisbdd is not supported in the one header library.\n");
    exit(1);
}

sbddextended_INLINE_FUNC
int bddiszbdd(bddp f)
{
    return bddisbdd(f);
}

#endif

#ifdef __cplusplus

// need to free the returned value
sbddextended_INLINE_FUNC
const char** sbddextended_strvector_to_array(const std::vector<std::string>& vec)
{
    const char** arr;

    arr = (const char**)malloc(vec.size() * sizeof(const char*));
    if (arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        arr[i] = vec[i].c_str();
    }
    return arr;
}

#endif

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

typedef struct tagsbddextended_MyDictNode {
    struct tagsbddextended_MyDictNode* left;
    struct tagsbddextended_MyDictNode* right;
    llint key;
    llint value;
} sbddextended_MyDictNode;

sbddextended_INLINE_FUNC
sbddextended_MyDictNode* sbddextended_MyDictNode_MakeNewNode(llint key,
                                                             llint value)
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
    size_t count;
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
    size_t stack_size;
    size_t debug_count;

    if (d->root == NULL) {
        assert(d->count == 0);
        return;
    }

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

    // free each node (not using a recursive function)
    while (sp >= 0) {
        node = node_stack[sp];
        op = op_stack[sp];

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
void sbddextended_MyDict_add(sbddextended_MyDict* d, llint key, llint value)
{
    sbddextended_MyDictNode* node;
    if (d->root == NULL) {
        d->root = sbddextended_MyDictNode_MakeNewNode(key, value);
        ++d->count;
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
                    ++d->count;
                    break;
                }
            } else { // key > node->key
                if (node->right != NULL) {
                    node = node->right;
                } else {
                    node->right = sbddextended_MyDictNode_MakeNewNode(key,
                                                                      value);
                    ++d->count;
                    break;
                }
            }
        }
    }
}

// returned value: 1 -> found, 0 -> not found
// The found value is stored into "value" argument.
sbddextended_INLINE_FUNC
int sbddextended_MyDict_find(const sbddextended_MyDict* d, llint key, llint* value)
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

sbddextended_INLINE_FUNC
void sbddextended_MyDict_copy(sbddextended_MyDict* dest,
                              const sbddextended_MyDict* src)
{
    sbddextended_MyDictNode** node_stack;
    sbddextended_MyDictNode** dest_node_stack;
    char* op_stack;
    char op;
    int sp;
    sbddextended_MyDictNode* node;
    sbddextended_MyDictNode* child;
    sbddextended_MyDictNode* dest_node;
    size_t stack_size;
    size_t debug_count;

    if (src->root == NULL) {
        assert(src->count == 0);
        dest->count = 0;
        dest->root = NULL;
        return;
    }

    assert((debug_count = 0) || 1);

    stack_size = src->count + 1;

    node_stack = (sbddextended_MyDictNode**)malloc(stack_size * sizeof(sbddextended_MyDictNode*));
    if (node_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    dest_node_stack = (sbddextended_MyDictNode**)malloc(stack_size * sizeof(sbddextended_MyDictNode*));
    if (node_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc(stack_size * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    dest->root = sbddextended_MyDictNode_MakeNewNode(src->root->key,
                                                     src->root->value);
    dest->root->key = src->root->key;
    dest->root->value = src->root->value;

    sp = 0;
    node_stack[sp] = src->root;
    dest_node_stack[sp] = dest->root;
    op_stack[sp] = 0;

    // free each node (not using a recursive function)
    while (sp >= 0) {
        node = node_stack[sp];
        dest_node = dest_node_stack[sp];
        op = op_stack[sp];

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
            dest_node_stack[sp] = sbddextended_MyDictNode_MakeNewNode(child->key,
                                                                      child->value);
            op_stack[sp] = 0;

            if (op == 0) {
                dest_node->left = dest_node_stack[sp];
            } else { // op == 1
                dest_node->right = dest_node_stack[sp];
            }
        } else {
            assert((++debug_count) || 1);
            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        }
    }
    assert(debug_count == src->count);
    dest->count = src->count;
}

sbddextended_INLINE_FUNC 
int sbddextended_readLine_inner(char* buf, FILE* fp)
{
    size_t n;
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

sbddextended_INLINE_FUNC
int sbddextended_write_inner(const char* buf, FILE* fp)
{
    if (fputs(buf, fp) == EOF) {
        return 0;
    }
    return 1;
}

sbddextended_INLINE_FUNC
int sbddextended_writeLine_inner(const char* buf, FILE* fp)
{
    if (fputs(buf, fp) == EOF || fputc('\n', fp) == EOF) {
        return 0;
    }
    return 1;
}

#ifdef __cplusplus

class WriteObject {
private:
    bool is_fstream_;
    bool is_ln_;
    std::ostream* ost_;

public:
    WriteObject(bool is_fstream, bool is_ln, std::ostream* ost)
        : is_fstream_(is_fstream), is_ln_(is_ln), ost_(ost) {}

    bool operator()(const char* buf, FILE* fp) const {
        if (is_fstream_) {
            if (!*ost_ || !(*ost_ << buf)) {
                return false;
            }
            if (is_ln_) {
                if (!*ost_ || !(*ost_ << '\n')) {
                    return false;
                }
            }
        } else {
            if (is_ln_) {
                return sbddextended_writeLine_inner(buf, fp) != 0;
            } else {
                return sbddextended_write_inner(buf, fp) != 0;
            }
        }
        return true;
    }
};

#else

sbddextended_INLINE_FUNC
int sbddextended_write(const char* buf, FILE* fp)
{
    return sbddextended_write_inner(buf, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeLine(const char* buf, FILE* fp)
{
    return sbddextended_writeLine_inner(buf, fp);
}

#endif

sbddextended_INLINE_FUNC int bddisnegative(bddp f)
{
    return (f & B_INV_MASK) ? 1 : 0;
}

sbddextended_INLINE_FUNC int bddisconstant(bddp f)
{
    return (f & B_CST_MASK) ? 1 : 0;
}

sbddextended_INLINE_FUNC bddp bddtakenot(bddp f)
{
    return f ^ B_INV_MASK;
}

sbddextended_INLINE_FUNC bddp bddaddnot(bddp f)
{
    return f | B_INV_MASK;
}

sbddextended_INLINE_FUNC bddp bdderasenot(bddp f)
{
    return f & ~B_INV_MASK;
}

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
    unused(f);
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC int bddisterminal(bddp f)
{
    return (f == bddempty || f == bddsingle || f == bddfalse || f == bddtrue) ? 1 : 0;
}

sbddextended_INLINE_FUNC bddvar bddgetvar(bddp f)
{
    return bddtop(f);
}

sbddextended_INLINE_FUNC bddvar bddgetlev(bddp f)
{
    return bddlevofvar(bddtop(f));
}

sbddextended_INLINE_FUNC bddp bddgetchild0b(bddp f)
{
    bddp g;
    g = bddat0(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild0z(bddp f)
{
    bddp g;
    g = bddoffset(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild0(bddp f)
{
    if (bddisbdd(f)) {
        return bddgetchild0b(f);
    } else {
        return bddgetchild0z(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild0braw(bddp f)
{
    if (bddisnegative(f)) {
        return bddtakenot(bddgetchild0b(f));
    } else {
        return bddgetchild0b(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild0zraw(bddp f)
{
    if (bddisnegative(f)) {
        return bddtakenot(bddgetchild0z(f));
    } else {
        return bddgetchild0z(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild0raw(bddp f)
{
    if (bddisbdd(f)) {
        return bddgetchild0braw(f);
    } else {
        return bddgetchild0zraw(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild1b(bddp f)
{
    bddp g;
    g = bddat1(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild1z(bddp f)
{
    bddp g;
    g = bddonset0(f, bddtop(f));
    bddfree(g);
    return g;
}

sbddextended_INLINE_FUNC bddp bddgetchild1(bddp f)
{
    if (bddisbdd(f)) {
        return bddgetchild1b(f);
    } else {
        return bddgetchild1z(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild1braw(bddp f)
{
    if (bddisnegative(f)) {
        return bddtakenot(bddgetchild1b(f));
    } else {
        return bddgetchild1b(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild1zraw(bddp f)
{
    return bddgetchild1z(f);
}

sbddextended_INLINE_FUNC bddp bddgetchild1raw(bddp f)
{
    if (bddisbdd(f)) {
        return bddgetchild1braw(f);
    } else {
        return bddgetchild1zraw(f);
    }
}

sbddextended_INLINE_FUNC bddp bddgetchildb(bddp f, int child)
{
    return (child != 0 ? bddgetchild1b(f) : bddgetchild0b(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildz(bddp f, int child)
{
    return (child != 0 ? bddgetchild1z(f) : bddgetchild0z(f));
}

sbddextended_INLINE_FUNC bddp bddgetchild(bddp f, int child)
{
    return (child != 0 ? bddgetchild1(f) : bddgetchild0(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildbraw(bddp f, int child)
{
    return (child != 0 ? bddgetchild1braw(f) : bddgetchild0braw(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildzraw(bddp f, int child)
{
    return (child != 0 ? bddgetchild1zraw(f) : bddgetchild0zraw(f));
}

sbddextended_INLINE_FUNC bddp bddgetchildraw(bddp f, int child)
{
    return (child != 0 ? bddgetchild1raw(f) : bddgetchild0raw(f));
}

sbddextended_INLINE_FUNC
bddp bddprimenot(bddvar v)
{
    bddp f, g;

    if (v > bddvarused()) {
        fprintf(stderr, "bddprimenot: Invalid VarID %d", v);
        exit(1);
    }
    f = bddprime(v);
    g = bddnot(f);
    bddfree(f);
    return g;
}

sbddextended_INLINE_FUNC
bddp bddgetsingleton(bddvar v)
{
    if (v > bddvarused()) {
        fprintf(stderr, "bddgetsingleton: Invalid VarID %d", v);
        exit(1);
    }
    return bddchange(bddsingle, v);
}

sbddextended_INLINE_FUNC
bddp bddgetsingleset(bddvar* vararr, int n)
{
    int i;
    bddp f, g;

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        assert(1 <= vararr[i] && vararr[i] <= bddvarused());
        g = bddchange(f, vararr[i]);
        bddfree(f);
        f = g;
    }
    return f;
}

sbddextended_INLINE_FUNC
bddp bddgetsinglesetv(int n, ...)
{
    int i;
    bddp f, g;
    va_list ap;
    bddvar v;

    if (n == 0) {
        return bddsingle;
    }

    va_start(ap, n);

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        v = va_arg(ap, bddvar);
        assert(1 <= v && v <= bddvarused());
        g = bddchange(f, v);
        bddfree(f);
        f = g;
    }
    va_end(ap);
    return f;
}


sbddextended_INLINE_FUNC
void sbddextended_sort_array(bddvar* arr, int n)
{
    int i, j;
    bddvar temp;

    for (i = n - 1; i >= 1; --i) {
        for (j = 0; j < i; ++j) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// must free the returned pointer
sbddextended_INLINE_FUNC
bddvar* sbddextended_getsortedarraybylevel_inner(const bddvar* vararr, int n)
{
    int i;
    bddvar* ar;

    ar = (bddvar*)malloc((size_t)n * sizeof(bddvar));
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        return NULL;
    }

    // translate varIDs to levels
    for (i = 0; i < n; ++i) {
        ar[i] = bddlevofvar(vararr[i]);
    }

    sbddextended_sort_array(ar, n);

    return ar;
}

sbddextended_INLINE_FUNC
bddp bddgetpowerset(const bddvar* vararr, int n)
{
    int i;
    bddp f, g, h;
    bddvar* ar;

    ar = sbddextended_getsortedarraybylevel_inner(vararr, n);
    if (ar == NULL) {
        return bddnull;
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
int bddismemberz_inner(bddp f, const bddvar* levarr, int n)
{
    bddp h;
    int sp;

    h = f;
    sp = n - 1;
    while (h != bddempty && h != bddsingle) {
        if (sp < 0 || bddgetlev(h) > levarr[sp]) {
            h = bddgetchild0z(h);
        } else if (bddgetlev(h) < levarr[sp]) { // return false
            break;
        } else {
            h = bddgetchild1z(h);
            --sp;
        }
    }
    return ((sp < 0 && h == bddsingle) ? 1 : 0);
}

sbddextended_INLINE_FUNC
int bddismemberz(bddp f, const bddvar* vararr, int n)
{
    int c;
    bddvar* ar;

    if (n == 0) {
        return bddisnegative(f);
    }

    ar = sbddextended_getsortedarraybylevel_inner(vararr, n);
    if (ar == NULL) {
        return -1;
    }
    c = bddismemberz_inner(f, ar, n);

    free(ar);

    return c;
}


// *************************** C++ version start *****************************

#ifdef __cplusplus

sbddextended_INLINE_FUNC bool IsNegative(const BDD& f)
{
    return bddisnegative(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool IsNegative(const ZBDD& f)
{
    return bddisnegative(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool IsConstant(const BDD& f)
{
    return bddisconstant(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool IsConstant(const ZBDD& f)
{
    return bddisconstant(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC BDD TakeNot(const BDD& f)
{
    return BDD_ID(bddtakenot(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD TakeNot(const ZBDD& f)
{
    return ZBDD_ID(bddtakenot(f.GetID()));
}

sbddextended_INLINE_FUNC BDD AddNot(const BDD& f)
{
    return BDD_ID(bddaddnot(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD AddNot(const ZBDD& f)
{
    return ZBDD_ID(bddaddnot(f.GetID()));
}

sbddextended_INLINE_FUNC BDD EraseNot(const BDD& f)
{
    return BDD_ID(bdderasenot(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD EraseNot(const ZBDD& f)
{
    return ZBDD_ID(bdderasenot(f.GetID()));
}

sbddextended_INLINE_FUNC bool Is64BitVersion()
{
    return bddis64bitversion() != 0;
}

sbddextended_INLINE_FUNC bool IsValid(const BDD& f)
{
    return bddisvalid(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool IsValid(const ZBDD& f)
{
    return bddisvalid(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool IsTerminal(const BDD& f)
{
    return bddisterminal(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool IsTerminal(const ZBDD& f)
{
    return bddisterminal(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bddvar GetVar(const BDD& f)
{
    return bddgetvar(f.GetID());
}

sbddextended_INLINE_FUNC bddvar GetVar(const ZBDD& f)
{
    return bddgetvar(f.GetID());
}

sbddextended_INLINE_FUNC bddvar GetLev(const BDD& f)
{
    return bddgetlev(f.GetID());
}

sbddextended_INLINE_FUNC bddvar GetLev(const ZBDD& f)
{
    return bddgetlev(f.GetID());
}

sbddextended_INLINE_FUNC BDD GetChild0(const BDD& f)
{
    return BDD_ID(bddgetchild0b(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD GetChild0(const ZBDD& f)
{
    return ZBDD_ID(bddgetchild0z(f.GetID()));
}

sbddextended_INLINE_FUNC BDD GetChild0Raw(const BDD& f)
{
    return BDD_ID(bddgetchild0braw(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD GetChild0Raw(const ZBDD& f)
{
    return ZBDD_ID(bddgetchild0zraw(f.GetID()));
}

sbddextended_INLINE_FUNC BDD GetChild1(const BDD& f)
{
    return BDD_ID(bddgetchild1b(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD GetChild1(const ZBDD& f)
{
    return ZBDD_ID(bddgetchild1z(f.GetID()));
}

sbddextended_INLINE_FUNC BDD GetChild1Raw(const BDD& f)
{
    return BDD_ID(bddgetchild1braw(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD GetChild1Raw(const ZBDD& f)
{
    return ZBDD_ID(bddgetchild1zraw(f.GetID()));
}

sbddextended_INLINE_FUNC BDD GetChild(const BDD& f, int child)
{
    return BDD_ID(bddgetchildb(f.GetID(), child));
}

sbddextended_INLINE_FUNC ZBDD GetChild(const ZBDD& f, int child)
{
    return ZBDD_ID(bddgetchildz(f.GetID(), child));
}

sbddextended_INLINE_FUNC BDD GetChildRaw(const BDD& f, int child)
{
    return BDD_ID(bddgetchildbraw(f.GetID(), child));
}

sbddextended_INLINE_FUNC ZBDD GetChildRaw(const ZBDD& f, int child)
{
    return ZBDD_ID(bddgetchildzraw(f.GetID(), child));
}

sbddextended_INLINE_FUNC ZBDD GetSingleton(bddvar v)
{
    return ZBDD_ID(bddgetsingleton(v));
}

sbddextended_INLINE_FUNC ZBDD GetSingleSet(const std::vector<bddvar>& vararr)
{
    bddp f, g;

    f = bddsingle;
    for (size_t i = 0; i < vararr.size(); ++i) {
        assert(1 <= vararr[i] && vararr[i] <= bddvarused());
        g = bddchange(f, vararr[i]);
        bddfree(f);
        f = g;
    }
    return ZBDD_ID(f);
}

sbddextended_INLINE_FUNC ZBDD GetSingleSet(int n, ...)
{
    int i;
    bddp f, g;
    va_list ap;
    bddvar v;

    if (n == 0) {
        return ZBDD_ID(1);
    }

    va_start(ap, n);

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        v = va_arg(ap, bddvar);
        assert(1 <= v && v <= bddvarused());
        g = bddchange(f, v);
        bddfree(f);
        f = g;
    }
    va_end(ap);
    return ZBDD_ID(f);
}

sbddextended_INLINE_FUNC ZBDD GetPowerSet(const std::vector<bddvar>& vararr)
{
    if (vararr.empty()) {
        return ZBDD(1);
    }

    int n = static_cast<int>(vararr.size());

    bddvar* ar = new bddvar[n];
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        return false;
    }

    for (int i = 0; i < n; ++i) {
        ar[i] = vararr[i];
    }

    bddp f = bddgetpowerset(ar, n);

    delete ar;

    return ZBDD_ID(f);
}

bool IsMemberZ(const ZBDD& f, const std::vector<bddvar>& vararr)
{
    if (vararr.empty()) {
        return bddisnegative(f.GetID());
    }

    int n = static_cast<int>(vararr.size());

    bddvar* ar = new bddvar[n];
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        return false;
    }

    // translate varIDs to levels
    for (int i = 0; i < n; ++i) {
        ar[i] = bddlevofvar(vararr[i]);
    }

    sbddextended_sort_array(ar, n);

    int b = bddismemberz_inner(f.GetID(), ar, n);

    delete ar;

    return b != 0;
}

#endif

typedef struct tagbddNodeIndex {
    int is_raw;
    sbddextended_MyDict* node_dict_arr;
    sbddextended_MyVector* level_vec_arr;
    llint* offset_arr;
    llint* count_arr;
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
    return (llint)index->offset_arr[0] - sbddextended_BDDNODE_START;
}

sbddextended_INLINE_FUNC
void bddNodeIndex_sizeeachlevel(const bddNodeIndex* index, bddvar* arr)
{
    int i;
    for (i = 1; i <= index->height; ++i) {
        arr[i] = (bddvar)(index->offset_arr[i - 1] - index->offset_arr[i]);
    }
}

sbddextended_INLINE_FUNC
llint bddNodeIndex_count(const bddNodeIndex* index)
{
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

    for (i = 1; i <= index->height; ++i) {
        sbddextended_MyVector_deinitialize(&index->level_vec_arr[i]);
        sbddextended_MyDict_deinitialize(&index->node_dict_arr[i]);
    }
    free(index->offset_arr);
    if (index->count_arr != NULL) {
        free(index->count_arr);
    }
}

sbddextended_INLINE_FUNC
void bddNodeIndex_copy(bddNodeIndex* dest,
                       const bddNodeIndex* src)
{
    dest->is_raw = src->is_raw;
    sbddextended_MyDict_copy(dest->node_dict_arr, src->node_dict_arr);
    sbddextended_MyVector_copy(dest->level_vec_arr, src->level_vec_arr);
    memcpy(dest->offset_arr, src->offset_arr, (size_t)(src->height + 1) * sizeof(llint));
    if (src->count_arr != NULL) {
        memcpy(dest->count_arr, src->count_arr, (size_t)src->offset_arr[0] * sizeof(llint));
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

    llint Size()
    {
        return bddNodeIndex_size(index_);
    }

    void SizeEachLevel(std::vector<bddvar>& arr)
    {
        arr.resize(index_->height + 1);
        for (int i = 1; i < index_->height; ++i) {
            arr[i] = (bddvar)(index_->offset_arr[i - 1] - index_->offset_arr[i]);
        }
    }

    llint Count()
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

sbddextended_INLINE_FUNC
void bddprintzbddelements_inner(FILE* fp, bddp f, const char* delim1,
                                const char* delim2, const char* var_name_map[]
#ifdef __cplusplus
                       , const WriteObject& sbddextended_write
#endif
                          )
{
    bddp* bddnode_stack;
    bddp g;
    char* op_stack;
    char op;
    int i, height, sp, is_first_delim1, is_first_delim2;
    bddp node, child;
    char buf[sbddextended_MAX_LINE];

    if (f == bddnull) {
        sbddextended_write("N", fp);
        return;
    } else if (f == bddempty) {
        sbddextended_write("E", fp);
        return;
    } else if (f == bddsingle) {
        return;
    }

    is_first_delim1 = 1;
    if (bddisnegative(f)) {
        // Output bddsingle first
        is_first_delim1 = 0;
        g = bddtakenot(f);
    } else {
        g = f;
    }

    height = (int)bddgetlev(g) + 1;
    bddnode_stack = (bddp*)malloc((size_t)height * sizeof(bddp));
    if (bddnode_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc((size_t)height * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    sp = 0;
    bddnode_stack[sp] = g;
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
            if (is_first_delim1 != 0) {
                is_first_delim1 = 0;
            } else {
                sprintf(buf, "%s", delim1);
                sbddextended_write(buf, fp);
            }
            is_first_delim2 = 1;
            for (i = 0; i < sp; ++i) {
                if (op_stack[i] == 0) {
                    if (is_first_delim2 != 0) {
                        is_first_delim2 = 0;
                    } else {
                        sprintf(buf, "%s", delim2);
                        sbddextended_write(buf, fp);
                    }
                    if (var_name_map != NULL) {
                        sprintf(buf, "%s", var_name_map[bddgetvar(bddnode_stack[i])]);
                    } else {
                        sprintf(buf, "%d", bddgetvar(bddnode_stack[i]));
                    }
                    sbddextended_write(buf, fp);
                }
            }

            --sp;
            if (sp < 0) {
                break;
            }
            ++op_stack[sp];
        } else {
            if (op <= 1) {
                if (op == 0) {
                    child = bddgetchild1z(node);
                } else {
                    child = bddgetchild0z(node);
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

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void PrintZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, wo);
}

sbddextended_INLINE_FUNC
void PrintZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(false, false, NULL);
    const char** arr = sbddextended_strvector_to_array(var_name_map);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void PrintZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, wo);
}

sbddextended_INLINE_FUNC
void PrintZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(true, false, &ost);
    const char** arr = sbddextended_strvector_to_array(var_name_map);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, wo);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, wo);
}

#else

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map);
}

#endif

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth_inner(FILE* fp, int is_hex, int root_level,
                                        int is_zdd
#ifdef __cplusplus
                             , const ReadLineObject& sbddextended_readLine
#endif
                             )
{
    int c, level, level_count = 1;
    llint i, id, lo, hi, line_count = 0;
    ullint idu, lou, hiu;
    bddvar var;
    char buf[sbddextended_MAX_LINE];
    bddp p, p0, p1, pf, pfn;
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
            sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);
            break;
        }
    }

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
            sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);
        } else {
            if (is_hex) {
                c = sscanf(buf, "%llx:%llx,%llx", &idu, &lou, &hiu);
                id = (llint)idu;
                lo = (llint)lou;
                hi = (llint)hiu;
            } else {
                c = sscanf(buf, "%lld:%lld,%lld", &id, &lo, &hi);
            }
            if (c < 3) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                exit(1);
            }
            if (id != (llint)lo_vec.count) {
                fprintf(stderr, "Format error in line %lld\n",
                        line_count);
                exit(1);
            }
            sbddextended_MyVector_add(&lo_vec, lo);
            sbddextended_MyVector_add(&hi_vec, hi);
        }
    }
    sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);

    if (root_level < 0) {
        root_level = level_count - 1;
    } else if (root_level < level_count - 1) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        exit(1);
    }

    while (bddvarused() < level_vec.count - 1) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc(lo_vec.count * sizeof(bddp));
    bddnode_buf[0] = (is_zdd == 0 ? bddfalse : bddempty);
    bddnode_buf[1] = (is_zdd == 0 ? bddtrue : bddsingle);

    for (i = (llint)lo_vec.count - 1; i >= sbddextended_BDDNODE_START; --i) {
        for (level = 1; level < (llint)level_vec.count; ++level) {
            if (sbddextended_MyVector_get(&level_vec, level - 1) <= i &&
                i < sbddextended_MyVector_get(&level_vec, level)) {
                break;
            }
        }
        assert(level < (llint)level_vec.count);
        assert((1 <= root_level - level + 1) && ((root_level - level + 1) <= (int)bddvarused()));
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { // BDD
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand(bddnode_buf[sbddextended_MyVector_get(&lo_vec, i)], pfn);
            p1 = bddand(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)], pf);
            bddnode_buf[i] = bddor(p0, p1);
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { // ZDD
            p0 = bddnode_buf[sbddextended_MyVector_get(&lo_vec, i)];
            p1 = bddchange(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)],
                           var);
            bddnode_buf[i] = bddunion(p0, p1);
            bddfree(p1);
        }
    }
    for (i = (llint)lo_vec.count - 1;
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
BDD ConstructBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD ConstructBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(true, &ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    return bddconstructbddfromfileknuth_inner(fp, is_hex,
                                              root_level, 0, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level)
{
    return bddconstructbddfromfileknuth_inner(fp, is_hex, root_level, 0);
}

#endif


#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD ConstructZBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD ConstructZBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(true, &ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo(false, NULL);
    return bddconstructbddfromfileknuth_inner(fp, is_hex,
                                              root_level, 1, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level)
{
    return bddconstructbddfromfileknuth_inner(fp, is_hex, root_level, 1);
}

#endif

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth_inner(FILE* fp, bddp f, int is_hex
#ifdef __cplusplus
                          , const WriteObject& sbddextended_writeLine
#endif
                             )
{
    int clevel, i;
    bddp node, n0, n1;
    llint id0, id1, k;
    bddNodeIndex* index;
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
    index = bddNodeIndex_makeIndexZWithoutCount(f);

    for (i = index->height; i >= 1; --i) {
        sprintf(ss, "#%d", index->height - i + 1);
        sbddextended_writeLine(ss, fp);
        for (k = 0; k < (llint)index->level_vec_arr[i].count; ++k) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], k);
            n0 = bddgetchild0z(node);
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
            n1 = bddgetchild1z(node);
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
            if (is_hex) {
                sprintf(ss, "%llx:%llx,%llx", index->offset_arr[i] + k, id0, id1);
            } else {
                sprintf(ss, "%lld:%lld,%lld", index->offset_arr[i] + k, id0, id1);
            }
            sbddextended_writeLine(ss, fp);
        }
    }
    bddNodeIndex_destruct(index);
    free(index);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void WriteZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtofileknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void WriteZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(true, true, &ost);
    bddwritezbddtofileknuth_inner(NULL, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth(FILE* fp, bddp f, int is_hex)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtofileknuth_inner(fp, f, (is_hex ? 1 : 0), wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritezbddtofileknuth(FILE* fp, bddp f, int is_hex)
{
    bddwritezbddtofileknuth_inner(fp, f, is_hex);
}

#endif

void bddoutputbddforgraphviz(FILE* fp, bddp f, bddNodeIndex* index)
{
    int i, k, c, clevel;
    llint cvalue;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    if (index == NULL) {
        is_making_index = 1;
        index = bddNodeIndex_makeIndexBWithoutCount(f);
    }

    fprintf(fp, "digraph {\n");
    fprintf(fp, "\tt0 [label = \"0\", shape = box];\n");
    fprintf(fp, "\tt1 [label = \"1\", shape = box];\n");

    fprintf(fp, "\tr%d [style = invis]\n", index->height);
    for (i = index->height; i >= 1; --i) {
        fprintf(fp, "\tr%d [style = invis];\n", i - 1);
        fprintf(fp, "\tr%d -> r%d [style = invis];\n", i, i - 1);
    }

    for (i = index->height; i >= 1; --i) {
        //fprintf(stderr, "%d", i);
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            //fprintf(stderr, " %d", node);
            fprintf(fp, "\tv%d_%lld [shape = circle];\n", i, (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                //fprintf(stderr, " %d", k);
                child = bddgetchildb(node, k);
                if (!bddisterminal(child)) {
                    clevel = (int)bddgetlev(child);
                    c = sbddextended_MyDict_find(&index->node_dict_arr[clevel],
                                                 (llint)child, &cvalue);
                    assert(c != 0);

                    fprintf(fp, "\tv%d_%lld -> v%d_%lld", i, (llint)j,
                            clevel, cvalue);
                    if (k == 0) {
                        fprintf(fp, " [style = dotted]");
                    }
                    fprintf(fp, ";\n");
                } else {
                    fprintf(fp, "\tv%d_%lld -> t%d", i, (llint)j,
                            (child == bddfalse ? 0 : 1));
                    if (k == 0) {
                        fprintf(fp, " [style = dotted]");
                    }
                    fprintf(fp, ";\n");
                }
            }
        }
        fprintf(fp, "\t{rank = same; r%d; ", i);
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            fprintf(fp, "v%d_%lld; ", i, (llint)j);
        }
        fprintf(fp, "}\n");
    }

    fprintf(fp, "\t{rank = same; r0; t0; t1; }\n");
    fprintf(fp, "}\n");

    if (is_making_index) {
        bddNodeIndex_destruct(index);
    }
}

// table size should be at least 2^n and vararr size should be at least n
bddp bddtruthtabletobdd(const unsigned char* table, const bddvar* vararr, int n)
{
    ullint n_pow, i;
    int j;
    bddp f, g, h, gp;
    if (n > 64) {
        fprintf(stderr, "n >= 64 not supported.\n");
        exit(1);
    }

    f = bddfalse;
    n_pow = (1llu << n);
    for (i = 0; i < n_pow; ++i) {
        if (table[i] != 0) {
            g = bddtrue;
            for (j = 0; j < n; ++j) {
                gp = bddprime(vararr[j]);
                if (((i >> j) & 1) == 0) {
                    h = bddnot(gp);
                    bddfree(gp);
                    gp = h;
                }
                h = bddand(g, gp);
                bddfree(g);
                bddfree(gp);
                g = h;
            }
            h = bddor(f, g);
            bddfree(f);
            bddfree(g);
            f = h;
        }
    }
    return f;
}



#ifdef __cplusplus
} // end of namespace sbddh
#endif

#endif // SBDD_HELPER_H

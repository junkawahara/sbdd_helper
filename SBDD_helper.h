/*
One header library for SAPPOROBDD C/C++ version
version 1.2.0

Copyright (c) 2017 -- 2024 Jun Kawahara

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef SBDD_HELPER_H
#define SBDD_HELPER_H

#ifdef __cplusplus

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <climits>
#include <cmath>
#include <cassert>
#include <cstdarg>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#if __cplusplus >= 201103L /* use rand() function */
#include <random>
#endif

#ifdef SBDDH_GMP
#include <gmp.h>
#include <gmpxx.h>
#endif

#else /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#endif /* __cplusplus */

#ifdef __cplusplus
namespace sbddh {
#ifdef SAPPOROBDD_PLUS_PLUS
using namespace sapporobdd;
#endif
#endif

typedef long long int llint;
typedef unsigned long long int ullint;

#define unused(a) (void)(a)

/* inline function qualifier for gcc */
/* if a compile error occurs, change the qualifier */
#define sbddextended_INLINE_FUNC static inline

#define sbddextended_BDDNODE_START 2
#define sbddextended_NUMBER_OF_CHILDREN 2

#define sbddextended_BUFSIZE 1024

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef SBDDH_GMP
    const mpz_class sbddextended_VALUE_ZERO(0);
    const mpz_class sbddextended_VALUE_ONE(1);
#else
    const llint sbddextended_VALUE_ZERO = 0;
    const llint sbddextended_VALUE_ONE = 1;
#endif

/* for compatibility */
#define getAllSetsIncluding getPowerSetIncluding
#define getAllPowerSetsIncluding getPowerSetIncluding
#define getAllPowerSetsNotIncluding getPowerSetNotIncluding
#define getAllSetsWithCard getPowerSetWithCard
#define getAllPowerSetsWithCard getPowerSetWithCard

#define bddconstructzbddfrombinary bddimportzbddasbinary
#define constructZBDDFromBinary importZBDDAsBinary

#define bddwritezbddtobinary bddexportzbddasbinary
#define writeZBDDToBinary exportZBDDAsBinary

#define constructZBDDFromGraphillion importZBDDAsGraphillion
#define bddwritebddforgraphillion bddexportbddasgraphillion
#define writeZBDDForGraphillion exportZBDDAsGraphillion

#define bddconstructbddfromfileknuth bddimportbddasknuth
#define bddconstructzbddfromfileknuth bddimportzbddasknuth
#define constructBDDFromFileKnuth importBDDAsKnuth
#define constructZBDDFromFileKnuth importZBDDAsKnuth

#define bddwritezbddtofileknuth bddexportzbddasknuth
#define writeZBDDToFileKnuth exportZBDDAsKnuth

#define bddwritebddforgraphviz bddexportbddasgraphviz
#define writeBDDForGraphviz exportBDDAsGraphviz
#define writeZBDDForGraphviz exportZBDDAsGraphviz



#ifdef __cplusplus

/* need to free the returned value */
sbddextended_INLINE_FUNC
const char** sbddextended_strVectorToArray(const std::vector<std::string>& vec)
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

/* XOR shift */
sbddextended_INLINE_FUNC
ullint sbddextended_getXRand(ullint* state)
{
    ullint v = *state;
    v ^= (v << 13);
    v ^= (v >> 7);
    v ^= (v << 17);
    *state = v;
    return v;
}

#ifdef __cplusplus

template<typename value_t>
double sbddh_divide(const value_t& op1, const value_t& op2)
{
    return static_cast<double>(op1) / static_cast<double>(op2);
}

#ifdef SBDDH_GMP

template<>
double sbddh_divide(const mpz_class& op1, const mpz_class& op2)
{
    mpf_class f1(op1.get_str());
    mpf_class f2(op2.get_str());
    mpf_class result;
    mpf_div(result.get_mpf_t(), f1.get_mpf_t(), f2.get_mpf_t());
    return result.get_d();
}

/* assume that v is non-negative */
sbddextended_INLINE_FUNC
ullint sbddh_mpz_to_ullint(const mpz_class& v)
{
    static mpz_class two32("4294967296"); /* 2^32 */
    static mpz_class two64("18446744073709551616"); /* 2^64 */
    assert(v >= 0);
    if (v < two32) {
        return static_cast<ullint>(v.get_ui());
    } else {
        mpz_class vv = v;
        if (v >= two64) { /* return remainder */
            mpz_class qq = v / two64;
            mpz_class rr = v - qq * two64;
            vv = rr;
        }
        assert(vv < two64);
        mpz_class q = vv / two32;
        mpz_class r = vv - q * two32;
        assert(q < two32);
        assert(r < two32);
        return static_cast<ullint>(q.get_ui()) * 4294967296ull
            + r.get_ui();
    }
}

sbddextended_INLINE_FUNC
mpz_class sbddh_llint_to_mpz(llint v)
{
    if (static_cast<llint>(INT_MIN) <= v &&
            v <= static_cast<llint>(INT_MAX)) { 
        return mpz_class(static_cast<int>(v));
    } else { /* mpz_class does not support the conversion
                from llint to mpz_class */
        std::stringstream ss;
        ss << v;
        return mpz_class(ss.str());
    }
}

sbddextended_INLINE_FUNC
mpz_class sbddh_ullint_to_mpz(ullint v)
{
    if (v <= static_cast<ullint>(UINT_MAX)) { 
        return mpz_class(static_cast<unsigned int>(v));
    } else { /* mpz_class does not support the conversion
                from ullint to mpz_class */
        std::stringstream ss;
        ss << v;
        return mpz_class(ss.str());
    }
}

#endif /* SBDDH_GMP */

template<typename value_t>
value_t sbddh_getZero()
{
    return value_t(0);
}

template<typename value_t>
value_t sbddh_getOne()
{
    return value_t(1);
}

template<typename value_t>
value_t sbddh_getCard(const ZBDD& f);

#ifdef SBDDH_GMP
template<typename value_t>
value_t sbddh_getValueFromMpz(const mpz_class& v);

template<>
inline
mpz_class sbddh_getValueFromMpz<mpz_class>(const mpz_class& v)
{
    return v;
}

template<>
inline
ullint sbddh_getValueFromMpz<ullint>(const mpz_class& v)
{
    return sbddh_mpz_to_ullint(v);
}

#else
template<typename value_t>
value_t sbddh_getValueFromMpz(value_t v);

template<>
inline
ullint sbddh_getValueFromMpz<ullint>(ullint v)
{
    return v;
}

#endif

#endif /* __cplusplus */


#ifdef __cplusplus /* C++ */
    #if __cplusplus >= 201103L /* C++11 */
        #define SBDDH_SNPRINTF_EXISTS
    #endif
#else /* C */
    #ifdef __STDC_VERSION__
        #if __STDC_VERSION__ >= 199901L /* C99 */
            #define SBDDH_SNPRINTF_EXISTS
        #endif
    #endif
#endif

#ifdef __clang_major__
    #if __clang_major__ >= 13 /* clang version 13 */
        #define SBDDH_SNPRINTF_EXISTS
    #endif
#endif

#ifdef SBDDH_SNPRINTF_EXISTS

/* We use the following macros instead of vsnprintf because passing */
/* a variable to the third argument of vsnprintf outputs warning */
/* "format string is not a string literal [-Wformat-nonliteral]". */

#define sbddextended_snprintf0(str, size, format) \
snprintf(str, size, format)
#define sbddextended_snprintf1(str, size, format, arg1) \
snprintf(str, size, format, arg1)
#define sbddextended_snprintf2(str, size, format, arg1, arg2) \
snprintf(str, size, format, arg1, arg2)
#define sbddextended_snprintf3(str, size, format, arg1, arg2, arg3) \
snprintf(str, size, format, arg1, arg2, arg3)
#define sbddextended_snprintf4(str, size, format, arg1, arg2, arg3, arg4) \
snprintf(str, size, format, arg1, arg2, arg3, arg4)
#define sbddextended_snprintf5(str, size, format, arg1, arg2, arg3, arg4, \
arg5) \
snprintf(str, size, format, arg1, arg2, arg3, arg4, arg5)

/* sbddextended_INLINE_FUNC
int sbddextended_sprintf(char *str, const char *format, ...)
{
    int v;
    va_list args;

    va_start(args, format);
    v = vsnprintf(str, sbddextended_BUFSIZE, format, args);
    va_end(args);
    return v;
} */

#else /* SBDDH_SNPRINTF_EXISTS */

#define sbddextended_snprintf0(str, size, format) \
sprintf(str, format)
#define sbddextended_snprintf1(str, size, format, arg1) \
sprintf(str, format, arg1)
#define sbddextended_snprintf2(str, size, format, arg1, arg2) \
sprintf(str, format, arg1, arg2)
#define sbddextended_snprintf3(str, size, format, arg1, arg2, arg3) \
sprintf(str, format, arg1, arg2, arg3)
#define sbddextended_snprintf4(str, size, format, arg1, arg2, arg3, arg4) \
sprintf(str, format, arg1, arg2, arg3, arg4)
#define sbddextended_snprintf5(str, size, format, arg1, arg2, arg3, arg4, \
arg5) \
sprintf(str, format, arg1, arg2, arg3, arg4, arg5)

/* sbddextended_INLINE_FUNC
int sbddextended_sprintf(char *str, const char *format, ...)
{
    int v;
    va_list args;

    va_start(args, format);
    v = sprintf(str, format, args);
    va_end(args);
    return v;
} */

#endif /* SBDDH_SNPRINTF_EXISTS */

#define sbddextended_MyVector_INITIAL_BUFSIZE 1024

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

#ifndef __cplusplus

typedef struct tagsbddextended_MyDictNode {
    struct tagsbddextended_MyDictNode* left;
    struct tagsbddextended_MyDictNode* right;
    llint key;
    llint value;
} sbddextended_MyDictNode;

sbddextended_INLINE_FUNC
sbddextended_MyDictNode* sbddextended_MyDictNode_makeNewNode(llint key,
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

#endif

typedef struct tagsbddextended_MyDict {
#ifdef __cplusplus
    std::map<llint, llint>* dict;
#endif
    size_t count;
#ifndef __cplusplus
    sbddextended_MyDictNode* root;
#endif
} sbddextended_MyDict;

sbddextended_INLINE_FUNC
void sbddextended_MyDict_initialize(sbddextended_MyDict* d)
{
#ifdef __cplusplus
    d->dict = new std::map<llint, llint>();
    d->count = 0;
#else
    d->count = 0;
    d->root = NULL;
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MyDict_deinitialize(sbddextended_MyDict* d)
{
#ifdef __cplusplus
    d->dict->clear();
    delete d->dict;
    d->count = 0;
#else
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

    /* free each node (not using a recursive function) */
    while (sp >= 0) {
        node = node_stack[sp];
        op = op_stack[sp];

        if (node == NULL) {
            op = 2;
        }

        while (op <= 1) {
            if (op == 0) {
                child = node->left;
            } else { /* op == 1 */
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
    free(op_stack);
    free(node_stack);
    assert(debug_count == d->count);
    d->count = 0;
    d->root = NULL;
#endif
}


sbddextended_INLINE_FUNC
void sbddextended_MyDict_add(sbddextended_MyDict* d, llint key, llint value)
{
#ifdef __cplusplus
    std::map<llint, llint>::const_iterator itor = d->dict->find(key);
    if (itor == d->dict->end()) { /* not found */
        ++d->count;
    }
    (*d->dict)[key] = value;
    assert(d->dict->size() == static_cast<size_t>(d->count));
#else
    sbddextended_MyDictNode* node;
    if (d->root == NULL) {
        d->root = sbddextended_MyDictNode_makeNewNode(key, value);
        ++d->count;
    } else {
        node = d->root;
        while (node != NULL) {
            if (node->key == key) { /* found */
                node->value = value;
                break;
            } else if (key < node->key) {
                if (node->left != NULL) {
                    node = node->left;
                } else {
                    node->left =
                        sbddextended_MyDictNode_makeNewNode(key, value);
                    ++d->count;
                    break;
                }
            } else { /* key > node->key */
                if (node->right != NULL) {
                    node = node->right;
                } else {
                    node->right =
                        sbddextended_MyDictNode_makeNewNode(key, value);
                    ++d->count;
                    break;
                }
            }
        }
    }
#endif
}

/* returned value: 1 -> found, 0 -> not found */
/* The found value is stored into "value" argument. */
sbddextended_INLINE_FUNC
int sbddextended_MyDict_find(const sbddextended_MyDict* d, llint key, llint* value)
{
#ifdef __cplusplus
    std::map<llint, llint>::const_iterator itor = d->dict->find(key);
    if (itor != d->dict->end()) {
        if (value != NULL) {
            *value = itor->second;
        }
        return 1;
    } else {
        return 0;
    }
#else
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
        } else {/* key > node->key */
            node = node->right;
        }
    }
    return 0;
#endif
}

sbddextended_INLINE_FUNC
void sbddextended_MyDict_copy(sbddextended_MyDict* dest,
                                const sbddextended_MyDict* src)
{
#ifdef __cplusplus
    *dest->dict = *src->dict;
    dest->count = src->count;
#else
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

    dest->root = sbddextended_MyDictNode_makeNewNode(src->root->key,
                                                        src->root->value);
    dest->root->key = src->root->key;
    dest->root->value = src->root->value;

    sp = 0;
    node_stack[sp] = src->root;
    dest_node_stack[sp] = dest->root;
    op_stack[sp] = 0;

    /* free each node (not using a recursive function) */
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
            } else { /* op == 1 */
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
            dest_node_stack[sp] =
                sbddextended_MyDictNode_makeNewNode(child->key, child->value);
            op_stack[sp] = 0;

            if (op == 0) {
                dest_node->left = dest_node_stack[sp];
            } else { /* op == 1 */
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
#endif
}


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

sbddextended_INLINE_FUNC
int sbddextended_readChar_inner(FILE* fp)
{
    return fgetc(fp);
}

/* The size of buf should be sbddextended_BUFSIZE. */
sbddextended_INLINE_FUNC
int sbddextended_readLine_inner(char* buf, FILE* fp)
{
    size_t n;
    char* p;
    p = fgets(buf, sbddextended_BUFSIZE, fp);
    if (p == NULL) {
        return 0;
    }
    n = strlen(buf);
    if (buf[n - 1] != '\n') {
        fprintf(stderr, "Each line must not exceed %d characters\n",
                sbddextended_BUFSIZE);
        exit(1);
    }
    return 1;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint8_inner(unsigned char* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned char), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint16_inner(unsigned short* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned short), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint32_inner(unsigned int* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned int), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint64_inner(ullint* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(ullint), (size_t)1, fp) != 0;
}


#ifdef __cplusplus

class ReadCharObject {
protected:
    static const int STREAM = 0;
    static const int FP = 1;
    static const int STRING = 2;
    typedef int Mode;

    Mode mode_;
    std::istream* ist_;
    const char* const st_;
    llint stpos_;
    const llint stlen_;

public:
    ReadCharObject()
        : mode_(FP), ist_(NULL), st_(NULL), stpos_(0), stlen_(0) { }

    ReadCharObject(std::istream* ist)
        : mode_(STREAM), ist_(ist), st_(NULL), stpos_(0), stlen_(0) { }

    ReadCharObject(const std::string& st)
        : mode_(STRING), ist_(NULL), st_(st.c_str()), stpos_(0),
          stlen_(static_cast<llint>(st.length())) { }

    ReadCharObject(const char* st)
        : mode_(STRING), ist_(NULL), st_(st), stpos_(0),
          stlen_((st_ != NULL) ? strlen(st_) : 0) { }

    int operator()(FILE* fp) {
        switch (mode_) {
        case STREAM:
            return ist_->get();
        case FP:
            return fgetc(fp);
        case STRING:
            if (stpos_ >= stlen_) {
                return -1;
            } else {
                ++stpos_;
                return st_[stpos_ - 1];
            }
        }
        return -1; /* never come here */
    }

    bool operator()(unsigned char* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(unsigned char));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint8_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }

    bool operator()(unsigned short* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(unsigned short));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint16_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }

    bool operator()(unsigned int* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(unsigned int));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint32_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }

    bool operator()(ullint* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(ullint));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint64_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }
};

class ReadLineObject : public ReadCharObject {
public:
    ReadLineObject()
        : ReadCharObject() { }

    ReadLineObject(std::istream* ist)
        : ReadCharObject(ist) { }

    ReadLineObject(const std::string& st)
        : ReadCharObject(st) { }

    ReadLineObject(const char* st)
        : ReadCharObject(st) { }

    bool operator()(char* buf, FILE* fp) {
        std::string str;
        switch (mode_) {
        case STREAM:
            if (!std::getline(*ist_, str)) {
                return false;
            }
            if (str.length() >= sbddextended_BUFSIZE - 1) {
                fprintf(stderr, "Each line must not exceed %d characters\n",
                    sbddextended_BUFSIZE);
                exit(1);
            }
            strcpy(buf, str.c_str());
            return true;
        case FP:
            return sbddextended_readLine_inner(buf, fp) != 0;
        case STRING:
            if (stpos_ >= stlen_) {
                return false;
            } else {
                llint start = stpos_;
                while (stpos_ < stlen_ && st_[stpos_] != '\n') {
                    ++stpos_;
                    if (stpos_ - start >= sbddextended_BUFSIZE - 1) {
                        fprintf(stderr, "Each line must not exceed %d characters\n",
                                sbddextended_BUFSIZE);
                        exit(1);
                    }
                }
                strncpy(buf, st_ + start, static_cast<size_t>(stpos_ - start));
                ++stpos_;
                return true;
            }
        }
        return false; /* never come here */
    }
};

#else

sbddextended_INLINE_FUNC
int sbddextended_readChar(FILE* fp)
{
    return sbddextended_readChar_inner(fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readLine(char* buf, FILE* fp)
{
    return sbddextended_readLine_inner(buf, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint8(unsigned char* v, FILE* fp)
{
    return sbddextended_readUint8_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint16(unsigned short* v, FILE* fp)
{
    return sbddextended_readUint16_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint32(unsigned int* v, FILE* fp)
{
    return sbddextended_readUint32_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint64(ullint* v, FILE* fp)
{
    return sbddextended_readUint64_inner(v, fp);
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

sbddextended_INLINE_FUNC
int sbddextended_writeUint8_inner(unsigned char v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned char), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint16_inner(unsigned short v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned short), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint32_inner(unsigned int v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned int), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint64_inner(ullint v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(ullint), (size_t)1, fp) != 0;
}

#ifdef __cplusplus

class WriteObject {
private:
    const bool is_fstream_;
    const bool is_ln_;
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
            assert(fp != NULL);
            if (is_ln_) {
                return sbddextended_writeLine_inner(buf, fp) != 0;
            } else {
                return sbddextended_write_inner(buf, fp) != 0;
            }
        }
        return true;
    }

    bool operator()(unsigned char v, FILE* fp) const {
        /*std::cerr << "uint8 " << (ullint)v << std::endl; */
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(unsigned char));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint8_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(unsigned short v, FILE* fp) const {
        /*std::cerr << "uint16 " << (ullint)v << std::endl; */
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(unsigned short));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint16_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(unsigned int v, FILE* fp) const {
        /*std::cerr << "uint32 " << (ullint)v << std::endl; */
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(unsigned int));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint32_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(ullint v, FILE* fp) const {
        /*std::cerr << "uint64 " << (ullint)v << std::endl; */
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(ullint));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint64_inner(v, fp) != 0;
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

sbddextended_INLINE_FUNC
int sbddextended_writeUint8(unsigned char v, FILE* fp)
{
    return sbddextended_writeUint8_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint16(unsigned short v, FILE* fp)
{
    return sbddextended_writeUint16_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint32(unsigned int v, FILE* fp)
{
    return sbddextended_writeUint32_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint64(ullint v, FILE* fp)
{
    return sbddextended_writeUint64_inner(v, fp);
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

sbddextended_INLINE_FUNC int bddisterminal(bddp f)
{
    return (f == bddempty || f == bddsingle || f == bddfalse || f == bddtrue) ? 1 : 0;
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

sbddextended_INLINE_FUNC int bddis64bitversion(void)
{
#ifdef SAPPOROBDD_PLUS_PLUS
#ifdef B_32
    return 0;
#else
    return 1;
#endif
#else
#ifdef B_64
    return 1;
#else
    return 0;
#endif
#endif
}

sbddextended_INLINE_FUNC void bddnewvarn(unsigned int n)
{
    unsigned int i;

    if (bddvarused() + n > bddvarmax) {
        fprintf(stderr, "The number of variables cannot exceed bddvarmax.\n");
        exit(1);
    }
    for (i = 0; i < n; ++i) {
        bddnewvar();
    }
}

sbddextended_INLINE_FUNC void bddnewvarrev(unsigned int n)
{
    unsigned int i;

    if (bddvarused() + n > bddvarmax) {
        fprintf(stderr, "The number of variables cannot exceed bddvarmax.\n");
        exit(1);
    }
    for (i = 0; i < n; ++i) {
        bddnewvaroflev(1);
    }
}

sbddextended_INLINE_FUNC int bddisvalid(bddp f)
{
    unused(f);
    fprintf(stderr, "not supported in the one header library\n");
    exit(1);
}

sbddextended_INLINE_FUNC bddp bddgetterminal(int terminal, int is_zbdd)
{
    assert(terminal == 0 || terminal == 1);
    if (is_zbdd != 0) {
        return (terminal == 0 ? bddempty : bddsingle);
    } else {
        return (terminal == 0 ? bddfalse : bddtrue);
    }
}

/* assume that f is ZBDD */
sbddextended_INLINE_FUNC int bddisemptymember(bddp f)
{
    return bddisnegative(f);
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

sbddextended_INLINE_FUNC bddp bddgetchild0g(bddp f, int is_zbdd, int is_raw)
{
    if (is_zbdd) {
        if (is_raw) {
            return bddgetchild0zraw(f);
        } else {
            return bddgetchild0z(f);
        }
    } else {
        if (is_raw) {
            return bddgetchild0braw(f);
        } else {
            return bddgetchild0b(f);
        }
    }
}

sbddextended_INLINE_FUNC bddp bddgetchild1g(bddp f, int is_zbdd, int is_raw)
{
    if (is_zbdd) {
        if (is_raw) {
            return bddgetchild1zraw(f);
        } else {
            return bddgetchild1z(f);
        }
    } else {
        if (is_raw) {
            return bddgetchild1braw(f);
        } else {
            return bddgetchild1b(f);
        }
    }
}

sbddextended_INLINE_FUNC bddp bddgetchildg(bddp f, int child,
                                            int is_zbdd, int is_raw)
{
    return (child != 0 ? bddgetchild1g(f, is_zbdd, is_raw) : bddgetchild0g(f, is_zbdd, is_raw));
}

sbddextended_INLINE_FUNC
bddp bddmakenodeb(bddvar v, bddp f0, bddp f1)
{
    bddp p, pn, g0, g1, g;

    if (v > bddvarused()) {
        fprintf(stderr, "bddmakenodeb: Invalid VarID %d", v);
        exit(1);
    }
    if (bddlevofvar(v) <= bddgetlev(f0)) {
        fprintf(stderr, "bddmakenodeb: The level of VarID %d "
            "must be larger than the level of f0\n", v);
        exit(1);
    }
    if (bddlevofvar(v) <= bddgetlev(f1)) {
        fprintf(stderr, "bddmakenodeb: The level of VarID %d "
            "must be larger than the level of f1\n", v);
        exit(1);
    }
    p = bddprime(v);
    pn = bddnot(p);
    g0 = bddand(f0, pn);
    bddfree(pn);
    g1 = bddand(f1, p);
    bddfree(p);
    g = bddor(g0, g1);
    bddfree(g0);
    bddfree(g1);
    return g;
}

sbddextended_INLINE_FUNC
bddp bddmakenodez(bddvar v, bddp f0, bddp f1)
{
    bddp g1, g;

    if (v > bddvarused()) {
        fprintf(stderr, "bddmakenodez: Invalid VarID %d", v);
        exit(1);
    }
    if (bddlevofvar(v) <= bddgetlev(f0)) {
        fprintf(stderr, "bddmakenodez: The level of VarID %d "
            "must be larger than the level of f0\n", v);
        exit(1);
    }
    if (bddlevofvar(v) <= bddgetlev(f1)) {
        fprintf(stderr, "bddmakenodez: The level of VarID %d "
            "must be larger than the level of f1\n", v);
        exit(1);
    }

    g1 = bddchange(f1, v);
    g = bddunion(f0, g1);
    bddfree(g1);
    return g;
}

sbddextended_INLINE_FUNC
bddp bddprimenot(bddvar v)
{
    bddp f;

    if (v > bddvarused()) {
        fprintf(stderr, "bddprimenot: Invalid VarID %d", v);
        exit(1);
    }
    f = bddprime(v);
    return bddaddnot(f);
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
    int i, j;
    bddp f, g;

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        assert(1 <= vararr[i] && vararr[i] <= bddvarused());
        for (j = 0; j < i; ++j) { /* check duplicate */
            if (vararr[j] == vararr[i]) {
                continue;
            }
        }
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

/* must free the returned pointer */
sbddextended_INLINE_FUNC
bddvar* sbddextended_getsortedarraybylevel_inner(const bddvar* vararr, int n)
{
    int i;
    bddvar* ar;

    assert(n >= 0);
    ar = (bddvar*)malloc((size_t)n * sizeof(bddvar));
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        return NULL;
    }

    /* translate varIDs to levels */
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
    bddvar v;

    ar = sbddextended_getsortedarraybylevel_inner(vararr, n);
    if (ar == NULL) {
        return bddnull;
    }

    f = bddsingle;
    for (i = 0; i < n; ++i) {
        v = bddvaroflev(ar[i]);
        assert(1 <= v && v <= bddvarused());
        g = bddchange(f, v);
        h = bddunion(f, g);
        bddfree(g);
        bddfree(f);
        f = h;
    }
    free(ar);
    return f;
}

sbddextended_INLINE_FUNC
bddp bddgetpowersetn(int n)
{
    bddp f, g, h;
    bddvar v;

    f = bddsingle;
    for (v = 1; v <= (bddvar)n; ++v) {
        assert(1 <= v && v <= bddvarused());
        g = bddchange(f, v);
        h = bddunion(f, g);
        bddfree(g);
        bddfree(f);
        f = h;
    }
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
        } else if (bddgetlev(h) < levarr[sp]) { /* return false */
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
    int i, c;
    bddvar* ar;

    assert(n >= 0);

    if (n == 0) {
        return bddisnegative(f);
    }

    for (i = 0; i < n; ++i) {
        if (!(1 <= vararr[i] && vararr[i] <= bddvarused())) {
            return 0;
        }
    }

    ar = sbddextended_getsortedarraybylevel_inner(vararr, n);
    if (ar == NULL) {
        return -1;
    }
    c = bddismemberz_inner(f, ar, n);

    free(ar);

    return c;
}

sbddextended_INLINE_FUNC
llint bddcountnodes_inner(bddp* dds, int n, int is_zbdd, int is_raw)
{
    int i;
    llint count = 0;
    bddp f, f0, f1;
    sbddextended_MyVector next_p;
    sbddextended_MySet visited;

    if (n == 0) {
        return 0;
    }
    for (i = 0; i < n; ++i) {
        if (dds[i] == bddnull) {
            return 0;
        }
    }
    sbddextended_MyVector_initialize(&next_p);
    sbddextended_MySet_initialize(&visited);

    for (i = n - 1; i >= 0; --i) {
        if (!bddisconstant(dds[i])
                && !sbddextended_MySet_exists(&visited, (llint)dds[i])) {
            sbddextended_MyVector_add(&next_p, (llint)dds[i]);
            sbddextended_MySet_add(&visited, (llint)dds[i]);
            ++count;
        }
    }

    while (next_p.count > 0) {
        f = (bddp)sbddextended_MyVector_get(&next_p, (llint)next_p.count - 1);
        sbddextended_MyVector_pop_back(&next_p);
        f0 = bddgetchild0g(f, is_zbdd, is_raw);
        if (!bddisconstant(f0)
                && !sbddextended_MySet_exists(&visited, (llint)f0)) {
            sbddextended_MyVector_add(&next_p, (llint)f0);
            sbddextended_MySet_add(&visited, (llint)f0);
            ++count;
        }
        f1 = bddgetchild1g(f, is_zbdd, is_raw);
        if (!bddisconstant(f1)
                && !sbddextended_MySet_exists(&visited, (llint)f1)) {
            sbddextended_MyVector_add(&next_p, (llint)f1);
            sbddextended_MySet_add(&visited, (llint)f1);
            ++count;
        }
    }
    sbddextended_MySet_deinitialize(&visited);
    sbddextended_MyVector_deinitialize(&next_p);
    return count;
}

sbddextended_INLINE_FUNC
llint bddcountnodes(bddp* dds, int n, int is_raw)
{
    int i, is_zbdd = -1, error = 0;
    for (i = 0; i < n; ++i) {
        if (!bddisconstant(dds[i])) {
            if (bddisbdd(dds[i])) {
                if (is_zbdd == 1) {
                    error = 1;
                    break;
                } else {
                    is_zbdd = 0;
                }
            } else { /* zbdd */
                if (is_zbdd == 0) {
                    error = 0;
                    break;
                } else {
                    is_zbdd = 1;
                }
            }
        }
    }
    if (error != 0) {
        fprintf(stderr, "bddcountnodes: both BDD and ZBDD exist.");
        exit(1);
    }
    return bddcountnodes_inner(dds, n, is_zbdd, is_raw);
}

/* *************************** C++ version start ***************************** */

#ifdef __cplusplus

sbddextended_INLINE_FUNC bool isNegative(const BDD& f)
{
    return bddisnegative(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isNegative(const ZBDD& f)
{
    return bddisnegative(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isConstant(const BDD& f)
{
    return bddisconstant(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isConstant(const ZBDD& f)
{
    return bddisconstant(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isTerminal(const BDD& f)
{
    return bddisterminal(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isTerminal(const ZBDD& f)
{
    return bddisterminal(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC BDD takeNot(const BDD& f)
{
    return BDD_ID(bddcopy(bddtakenot(f.GetID())));
}

sbddextended_INLINE_FUNC ZBDD takeNot(const ZBDD& f)
{
    return ZBDD_ID(bddcopy(bddtakenot(f.GetID())));
}

sbddextended_INLINE_FUNC BDD addNot(const BDD& f)
{
    return BDD_ID(bddcopy(bddaddnot(f.GetID())));
}

sbddextended_INLINE_FUNC ZBDD addNot(const ZBDD& f)
{
    return ZBDD_ID(bddcopy(bddaddnot(f.GetID())));
}

sbddextended_INLINE_FUNC BDD eraseNot(const BDD& f)
{
    return BDD_ID(bddcopy(bdderasenot(f.GetID())));
}

sbddextended_INLINE_FUNC ZBDD eraseNot(const ZBDD& f)
{
    return ZBDD_ID(bddcopy(bdderasenot(f.GetID())));
}

sbddextended_INLINE_FUNC bool is64BitVersion()
{
    return bddis64bitversion() != 0;
}

sbddextended_INLINE_FUNC void SBDDH_NewVar(unsigned int n)
{
    bddnewvarn(n);
}

sbddextended_INLINE_FUNC void SBDDH_NewVarRev(unsigned int n)
{
    bddnewvarrev(n);
}

sbddextended_INLINE_FUNC bool isValid(const BDD& f)
{
    return bddisvalid(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isValid(const ZBDD& f)
{
    return bddisvalid(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isEmptyMember(const ZBDD& f)
{
    return bddisemptymember(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bddvar getVar(const BDD& f)
{
    return bddgetvar(f.GetID());
}

sbddextended_INLINE_FUNC bddvar getVar(const ZBDD& f)
{
    return bddgetvar(f.GetID());
}

sbddextended_INLINE_FUNC bddvar getLev(const BDD& f)
{
    return bddgetlev(f.GetID());
}

sbddextended_INLINE_FUNC bddvar getLev(const ZBDD& f)
{
    return bddgetlev(f.GetID());
}

sbddextended_INLINE_FUNC BDD getChild0(const BDD& f)
{
    bddp g;
    g = bddat0(f.GetID(), f.Top());
    return BDD_ID(g);
}

sbddextended_INLINE_FUNC ZBDD getChild0(const ZBDD& f)
{
    bddp g;
    g = bddoffset(f.GetID(), f.Top());
    return ZBDD_ID(g);
}

sbddextended_INLINE_FUNC BDD getChild0Raw(const BDD& f)
{
    bddp g;
    g = bddat0(f.GetID(), f.Top());
    if (isNegative(f)) {
        g = bddtakenot(g);
    }
    return BDD_ID(g);
}

sbddextended_INLINE_FUNC ZBDD getChild0Raw(const ZBDD& f)
{
    bddp g;
    g = bddoffset(f.GetID(), f.Top());
    if (isNegative(f)) {
        g = bddtakenot(g);
    }
    return ZBDD_ID(g);
}

sbddextended_INLINE_FUNC BDD getChild1(const BDD& f)
{
    bddp g;
    g = bddat1(f.GetID(), f.Top());
    return BDD_ID(g);
}

sbddextended_INLINE_FUNC ZBDD getChild1(const ZBDD& f)
{
    bddp g;
    g = bddonset0(f.GetID(), f.Top());
    return ZBDD_ID(g);
}

sbddextended_INLINE_FUNC BDD getChild1Raw(const BDD& f)
{
    bddp g;
    g = bddat1(f.GetID(), f.Top());
    if (isNegative(f)) {
        g = bddtakenot(g);
    }
    return BDD_ID(g);
}

sbddextended_INLINE_FUNC ZBDD getChild1Raw(const ZBDD& f)
{
    return getChild1(f);
}

sbddextended_INLINE_FUNC BDD getChild(const BDD& f, int child)
{
    if (child == 1) {
        return getChild1(f);
    } else {
        return getChild0(f);
    }
}

sbddextended_INLINE_FUNC ZBDD getChild(const ZBDD& f, int child)
{
    if (child == 1) {
        return getChild1(f);
    } else {
        return getChild0(f);
    }
}

sbddextended_INLINE_FUNC BDD getChildRaw(const BDD& f, int child)
{
    if (child == 1) {
        return getChild1Raw(f);
    } else {
        return getChild0Raw(f);
    }
}

sbddextended_INLINE_FUNC ZBDD getChildRaw(const ZBDD& f, int child)
{
    if (child == 1) {
        return getChild1Raw(f);
    } else {
        return getChild0Raw(f);
    }
}

sbddextended_INLINE_FUNC
BDD makeNode(bddvar v, const BDD& f0, const BDD& f1)
{
    assert(bddlevofvar(v) > getLev(f0));
    assert(bddlevofvar(v) > getLev(f1));
    return (f0 & ~BDDvar(v)) | (f1 & BDDvar(v));
}

sbddextended_INLINE_FUNC
ZBDD makeNode(bddvar v, const ZBDD& f0, const ZBDD& f1)
{
    assert(bddlevofvar(v) > getLev(f0));
    assert(bddlevofvar(v) > getLev(f1));
    return f0 + f1.Change(v);
}

sbddextended_INLINE_FUNC BDD getPrimeNot(bddvar v)
{
    return BDD_ID(bddprimenot(v));
}

sbddextended_INLINE_FUNC ZBDD getSingleton(bddvar v)
{
    return ZBDD_ID(bddgetsingleton(v));
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getSingleSet(const T& variables)
{
    /* use set to remove duplicated elements */
    std::set<bddvar> s(variables.begin(), variables.end());

    bddp f, g;

    f = bddsingle;
    for (std::set<bddvar>::const_iterator itor = s.begin();
         itor != s.end(); ++itor) {
        assert(1 <= *itor && *itor <= bddvarused());
        g = bddchange(f, *itor);
        bddfree(f);
        f = g;
    }
    return ZBDD_ID(f);
}

sbddextended_INLINE_FUNC ZBDD getSingleSet(int n, ...)
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

/* need to delete[] the returned pointer */
template<typename T>
sbddextended_INLINE_FUNC bddvar* containerToArray(const T& variables,
                                                  int* n)
{
    *n = 0;

    for (typename T::const_iterator itor = variables.begin();
         itor != variables.end(); ++itor) {
        ++(*n);
    }

    if (*n == 0) {
        return NULL;
    }

    bddvar* ar = new bddvar[*n];
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    int c = 0;
    for (typename T::const_iterator itor = variables.begin();
         itor != variables.end(); ++itor) {
        ar[c] = *itor;
        ++c;
    }
    return ar;
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getPowerSet(const T& variables)
{
    int n;
    bddvar* ar = containerToArray(variables, &n);
    if (n == 0) {
        return ZBDD(0);
    }

    bddp f = bddgetpowerset(ar, n);

    delete[] ar;

    return ZBDD_ID(f);
}

sbddextended_INLINE_FUNC ZBDD getPowerSet(int n)
{
    bddp f = bddgetpowersetn(n);
    return ZBDD_ID(f);
}

template<typename T1, typename T2>
sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding_inner(const T1& base_variables,
                                                        const T2& target_variables)
{
    ZBDD f = getPowerSet(base_variables);

    bddvar c = 0;
    for (typename T2::const_iterator itor = target_variables.begin();
         itor != target_variables.end(); ++itor) {
        c = *itor;
        if (std::find(base_variables.begin(), base_variables.end(), c) !=
            base_variables.end()) { /* c found */
            f = f.OnSet(c);
        } else {
            f = f.Change(c);
        }
    }
    return f;
}


template<typename T>
sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding(const T& base_variables,
                                                  const std::vector<bddvar>& target_variables)
{
    return getPowerSetIncluding_inner(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding(const T& base_variables,
                                                  const std::set<bddvar>& target_variables)
{
    return getPowerSetIncluding_inner(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding(const T& base_variables,
                                                  bddvar v)
{
    std::vector<bddvar> target_variables;
    target_variables.push_back(v);

    return getPowerSetIncluding(base_variables, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding(int n,
                                                       const std::vector<bddvar>& target_variables)
{
    std::vector<bddvar> base_variables;
    for (int v = 1; v <= n; ++v) {
        base_variables.push_back(v);
    }

    return getPowerSetIncluding(base_variables, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding(int n,
                                                       const std::set<bddvar>& target_variables)
{
    std::vector<bddvar> base_variables;
    for (int v = 1; v <= n; ++v) {
        base_variables.push_back(v);
    }

    return getPowerSetIncluding(base_variables, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getPowerSetIncluding(int n, int v)
{
    std::vector<bddvar> target_variables;
    target_variables.push_back(v);

    return getPowerSetIncluding(n, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getPowerSetNotIncluding_inner(int n,
                                                                const T& target_variables)
{
    ZBDD f = getPowerSet(n);

    for (typename T::const_iterator itor = target_variables.begin();
         itor != target_variables.end(); ++itor) {
        f = f.OffSet(*itor);
    }
    return f;
}

sbddextended_INLINE_FUNC ZBDD getPowerSetNotIncluding(int n,
                                                          const std::vector<bddvar>& target_variables)
{
    return getPowerSetNotIncluding_inner(n, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getPowerSetNotIncluding(int n,
                                                          const std::set<bddvar>& target_variables)
{
    return getPowerSetNotIncluding_inner(n, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getPowerSetNotIncluding(int n, int v)
{
    ZBDD f = getPowerSet(n);
    f = f.OffSet(v);
    return f;
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getPowerSetWithCard(const T& variables, int k)
{
    int n;
    bddvar* ar = containerToArray(variables, &n);
    if (n < k) {
        if (ar != NULL) {
            delete[] ar;
        }
        return ZBDD(0);
    }
    for (int i = 0; i < n; ++i) {
        ar[i] = bddlevofvar(ar[i]);
    }

    sbddextended_sort_array(ar, n);

    std::vector<ZBDD> current;
    std::vector<ZBDD> next(static_cast<size_t>(k) + 1);

    for (int i = 0; i < k; ++i) {
        current.push_back(ZBDD(0));
    }
    current.push_back(ZBDD(1));

    for (int i = 0; i < n; ++i) {
        int v = bddvaroflev(ar[i]);
        for (int j = 0; j <= std::min(n - i - 1, k); ++j) {
            if (j < k) {
                next[j] = current[j]
                    + current[static_cast<size_t>(j) + 1].Change(v);
            } else {
                next[j] = current[j];
            }
        }
        current = next;
    }
    delete[] ar;
    return current[0];
}

sbddextended_INLINE_FUNC ZBDD getPowerSetWithCard(int n, int k)
{
    std::vector<bddvar> variables;
    for (int v = 1; v <= n; ++v) {
        variables.push_back(v);
    }
    return getPowerSetWithCard(variables, k);
}


template<typename T>
sbddextended_INLINE_FUNC ZBDD makeDontCare(const ZBDD& f, const T& variables)
{
    ZBDD g = f;
    for (typename T::const_iterator itor = variables.begin();
         itor != variables.end(); ++itor) {
        if (!(1 <= *itor && *itor <= bddvarused())) {
            return ZBDD(-1);
        }
        g = g + g.Change(*itor);
    }
    return g;
}

template<typename T>
sbddextended_INLINE_FUNC bool isMemberZ(const ZBDD& f, const T& variables)
{
    int n = 0;

    for (typename T::const_iterator itor = variables.begin();
         itor != variables.end(); ++itor) {
        if (!(1 <= *itor && *itor <= bddvarused())) {
            return false;
        }
        ++n;
    }

    if (n == 0) {
        return bddisnegative(f.GetID());
    }

    bddvar* ar = new bddvar[n];
    if (ar == NULL) {
        fprintf(stderr, "out of memory\n");
        return false;
    }

    /* translate varIDs to levels */
    int c = 0;
    for (typename T::const_iterator itor = variables.begin();
         itor != variables.end(); ++itor) {
        ar[c] = bddlevofvar(*itor);
        ++c;
    }

    sbddextended_sort_array(ar, n);

    int b = bddismemberz_inner(f.GetID(), ar, n);

    delete[] ar;

    return b != 0;
}

template<typename T>
sbddextended_INLINE_FUNC bool isMember(const ZBDD& f, const T& variables)
{
    return isMemberZ(f, variables);
}

sbddextended_INLINE_FUNC
llint countNodes(const std::vector<bddp>& dds, bool is_raw = false)
{
    bddp* bps = new bddp[dds.size()];
    int i = 0;
    for (std::vector<bddp>::const_iterator itor = dds.begin();
            itor != dds.end(); ++itor) {
        bps[i] = *itor;
        ++i;
    }
    llint count = bddcountnodes(bps, static_cast<int>(dds.size()),
                                (is_raw ? 1 : 0));
    delete[] bps;
    return count;
}

sbddextended_INLINE_FUNC
llint countNodes(const std::set<bddp>& dds, bool is_raw = false)
{
    bddp* bps = new bddp[dds.size()];
    int i = 0;
    for (std::set<bddp>::const_iterator itor = dds.begin();
            itor != dds.end(); ++itor) {
        bps[i] = *itor;
        ++i;
    }
    llint count = bddcountnodes(bps, static_cast<int>(dds.size()),
                                (is_raw ? 1 : 0));
    delete[] bps;
    return count;
}

sbddextended_INLINE_FUNC
llint countNodes(const std::vector<BDD>& dds, bool is_raw = false)
{
    bddp* bps = new bddp[dds.size()];
    int i = 0;
    for (std::vector<BDD>::const_iterator itor = dds.begin();
            itor != dds.end(); ++itor) {
        bps[i] = itor->GetID();
        ++i;
    }
    llint count = bddcountnodes_inner(bps, static_cast<int>(dds.size()),
                                        0, (is_raw ? 1 : 0));
    delete[] bps;
    return count;
}

sbddextended_INLINE_FUNC
llint countNodes(const std::set<BDD>& dds, bool is_raw = false)
{
    bddp* bps = new bddp[dds.size()];
    int i = 0;
    for (std::set<BDD>::const_iterator itor = dds.begin();
            itor != dds.end(); ++itor) {
        bps[i] = itor->GetID();
        ++i;
    }
    llint count = bddcountnodes_inner(bps, static_cast<int>(dds.size()),
                                        0, (is_raw ? 1 : 0));
    delete[] bps;
    return count;
}

sbddextended_INLINE_FUNC
llint countNodes(const std::vector<ZBDD>& dds, bool is_raw = false)
{
    bddp* bps = new bddp[dds.size()];
    int i = 0;
    for (std::vector<ZBDD>::const_iterator itor = dds.begin();
            itor != dds.end(); ++itor) {
        bps[i] = itor->GetID();
        ++i;
    }
    llint count = bddcountnodes_inner(bps, static_cast<int>(dds.size()),
                                        1, (is_raw ? 1 : 0));
    delete[] bps;
    return count;
}

sbddextended_INLINE_FUNC
llint countNodes(const std::set<ZBDD>& dds, bool is_raw = false)
{
    bddp* bps = new bddp[dds.size()];
    int i = 0;
    for (std::set<ZBDD>::const_iterator itor = dds.begin();
            itor != dds.end(); ++itor) {
        bps[i] = itor->GetID();
        ++i;
    }
    llint count = bddcountnodes_inner(bps, static_cast<int>(dds.size()),
                                        1, (is_raw ? 1 : 0));
    delete[] bps;
    return count;
}

#if __cplusplus >= 201103L

class DDUtilityCpp11 {
public:
    template <typename T>
    static ZBDD getUniformlyRandomZBDD(int level, T& random_engine)
    {
        if (level > 0) {
            return makeNode(bddvaroflev(level),
                            getUniformlyRandomZBDD(level - 1,
                                                    random_engine),
                            getUniformlyRandomZBDD(level - 1,
                                                    random_engine));
        } else {
            std::uniform_int_distribution<int> dist(0, 1);
            if (dist(random_engine) != 0) {
                return ZBDD(1);
            } else {
                return ZBDD(0);
            }
        }
    }

    template <typename T>
    static ZBDD getRandomZBDDWithCard(int level, llint card, T& random_engine)
    {
        ZBDD f(0);
        std::set<bddvar> s;
        std::uniform_int_distribution<int> dist(0, 1);

        while ((llint)f.Card() < card) {
            for (int lev = 1; lev <= level; ++lev) {
                if (dist(random_engine) != 0) {
                    s.insert(bddvaroflev(lev));
                }
            }
            f += getSingleSet(s);
            s.clear();
        }

        return f;
    }
};

template <typename T>
sbddextended_INLINE_FUNC
ZBDD getUniformlyRandomZBDD(int level, T& random_engine)
{
    return DDUtilityCpp11::getUniformlyRandomZBDD(level, random_engine);
}

template <typename T>
sbddextended_INLINE_FUNC
ZBDD getRandomZBDDWithCard(int level, llint card, T& random_engine)
{
    return DDUtilityCpp11::getRandomZBDDWithCard(level, card, random_engine);
}

#endif /* __cplusplus >= 201103L */

class DDUtility {
public:
    static BDD getUniformlyRandomBDDX(int level, ullint* rand_state)
    {
        if (level > 0) {
            return makeNode(bddvaroflev(level),
                            getUniformlyRandomBDDX(level - 1,
                                                    rand_state),
                            getUniformlyRandomBDDX(level - 1,
                                                    rand_state));
        } else {
            ullint v = sbddextended_getXRand(rand_state);
            if (((v >> 19) & 1u) != 0) {
                return BDD(1);
            } else {
                return BDD(0);
            }
        }
    }

    static ZBDD getUniformlyRandomZBDDX(int level, ullint* rand_state)
    {
        if (level > 0) {
            return makeNode(bddvaroflev(level),
                            getUniformlyRandomZBDDX(level - 1,
                                                    rand_state),
                            getUniformlyRandomZBDDX(level - 1,
                                                    rand_state));
        } else {
            ullint v = sbddextended_getXRand(rand_state);
            if (((v >> 19) & 1u) != 0) {
                return ZBDD(1);
            } else {
                return ZBDD(0);
            }
        }
    }

    static ZBDD getRandomZBDDWithCardX(int level, llint card, ullint* rand_state)
    {
        ZBDD f(0);
        std::set<bddvar> s;

        while ((llint)f.Card() < card) {
            for (int lev = 1; lev <= level; ++lev) {
                ullint v = sbddextended_getXRand(rand_state);
                if (((v >> 19) & 1u) != 0) {
                    s.insert(bddvaroflev(lev));
                }
            }
            f += getSingleSet(s);
            s.clear();
        }

        return f;
    }
};

sbddextended_INLINE_FUNC
BDD getUniformlyRandomBDDX(int level, ullint* rand_state)
{
    return DDUtility::getUniformlyRandomBDDX(level, rand_state);
}

sbddextended_INLINE_FUNC
ZBDD getUniformlyRandomZBDDX(int level, ullint* rand_state)
{
    return DDUtility::getUniformlyRandomZBDDX(level, rand_state);
}

sbddextended_INLINE_FUNC
ZBDD getRandomZBDDWithCardX(int level, llint card, ullint* rand_state)
{
    return DDUtility::getRandomZBDDWithCardX(level, card, rand_state);
}

sbddextended_INLINE_FUNC
BDD exampleBdd(ullint kind = 0ull)
{
    /* kind == 0 cannot be used for rand_state */
    kind += 1;
    ullint rand_state = kind;
    ullint v = sbddextended_getXRand(&rand_state);
    int size = static_cast<int>((v % 6) + 3);
    while (BDD_VarUsed() < size) {
        BDD_NewVar();
    }
    return DDUtility::getUniformlyRandomBDDX(size, &rand_state);
}

sbddextended_INLINE_FUNC
ZBDD exampleZbdd(ullint kind = 0ull)
{
    /* kind == 0 cannot be used for rand_state */
    kind += 1;
    ullint rand_state = kind;
    ullint v = sbddextended_getXRand(&rand_state);
    int size = static_cast<int>((v % 6) + 3);
    while (BDD_VarUsed() < size) {
        BDD_NewVar();
    }
    return DDUtility::getUniformlyRandomZBDDX(size, &rand_state);
}

#ifdef SBDDH_BDDCT

sbddextended_INLINE_FUNC
bool weightRange_initialize(BDDCT* bddct, bddvar lev,
    const std::vector<llint>& weights)
{
    for (size_t i = 0; i < weights.size(); ++i) {
        if (weights[i] >= (1ll << 32)) {
            std::cerr << "Each weight should be less than 2^32" << std::endl;
            return false;
        }
    }

    bddct->Alloc(lev);
    for (bddvar le = 1; le <= lev; ++le) {
        const int var = bddvaroflev(le);
        if (static_cast<int>(weights.size()) <= var) {
            std::cerr << "The size of weights should be larger than "
                "the maximum variable number in f." << std::endl;
            return false;
        }
        bddct->SetCostOfLev(le, static_cast<int>(weights[var]));
    }
    return true;
}

sbddextended_INLINE_FUNC
ZBDD weightRange(const ZBDD& f, llint lower_bound, llint upper_bound, const std::vector<llint>& weights)
{
    if (lower_bound >= (1ll << 32)) {
        std::cerr << "lower_bound should be less than 2^32" << std::endl;
        return ZBDD(-1);
    }
    if (upper_bound >= (1ll << 32)) {
        std::cerr << "upper_bound should be less than 2^32" << std::endl;
        return ZBDD(-1);
    }
    BDDCT bddct;
    const int lev = getLev(f);
    if (!weightRange_initialize(&bddct, lev, weights)) {
        return ZBDD(-1);
    }

    ZBDD z = bddct.ZBDD_CostLE(f, static_cast<int>(upper_bound));
    if (lower_bound > LLONG_MIN) {
        z -= bddct.ZBDD_CostLE(f, static_cast<int>(lower_bound - 1));
    }
    return z;
}

sbddextended_INLINE_FUNC
ZBDD weightLE(const ZBDD& f, llint bound, const std::vector<llint>& weights)
{
    return weightRange(f, LLONG_MIN, bound, weights);
}

sbddextended_INLINE_FUNC
ZBDD weightLT(const ZBDD& f, llint bound, const std::vector<llint>& weights)
{
    return weightLE(f, bound - 1, weights);
}

sbddextended_INLINE_FUNC
ZBDD weightGE(const ZBDD& f, llint bound, const std::vector<llint>& weights)
{
    std::vector<llint> negative_weights(weights);
    for (size_t i = 0; i < weights.size(); ++i) {
        negative_weights[i] = -negative_weights[i];
    }
    return weightLE(f, -bound, negative_weights);
}

sbddextended_INLINE_FUNC
ZBDD weightGT(const ZBDD& f, llint bound, const std::vector<llint>& weights)
{
    return weightGE(f, bound + 1, weights);
}

sbddextended_INLINE_FUNC
ZBDD weightEQ(const ZBDD& f, llint bound, const std::vector<llint>& weights)
{
    return weightRange(f, bound, bound, weights);
}

sbddextended_INLINE_FUNC
ZBDD weightNE(const ZBDD& f, llint bound, const std::vector<llint>& weights)
{
    return f - weightEQ(f, bound, weights);
}

#endif /* SBDDH_BDDCT */

#endif

typedef struct tagbddNodeIndex {
    int is_raw;
    int is_zbdd;
    /* All of the following four pointers are NULL if f is a terminal or bddnull. */
    sbddextended_MyDict* node_dict_arr;
    sbddextended_MyVector* level_vec_arr; /* stores all nodes at level i */
    llint* offset_arr;
    ullint* count_arr; /* array representing the number of solutions for node i */
    int height;
    bddp f;
} bddNodeIndex;

sbddextended_INLINE_FUNC
bddNodeIndex* bddNodeIndex_makeIndexWithoutCount_inner(bddp f, int is_raw, int is_zbdd)
{
    int i, k, level;
    size_t j;
    bddp node, child;
    bddNodeIndex* node_index;

    node_index = (bddNodeIndex*)malloc(sizeof(bddNodeIndex));
    if (node_index == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node_index->is_raw = (is_raw != 0 ? 1 : 0);
    node_index->f = f;
    node_index->height = (int)bddgetlev(f);
    node_index->is_zbdd = (is_zbdd != 0 ? 1 : 0);

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        node_index->node_dict_arr = NULL;
        node_index->level_vec_arr = NULL;
        node_index->offset_arr = NULL;
        node_index->count_arr = NULL;
        return node_index;
    }

    if (is_raw) {
        f = bdderasenot(f);
    }

    node_index->node_dict_arr = (sbddextended_MyDict*)malloc(
                            (size_t)(node_index->height + 1) * sizeof(sbddextended_MyDict));
    if (node_index->node_dict_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node_index->level_vec_arr = (sbddextended_MyVector*)malloc(
                            (size_t)(node_index->height + 1) * sizeof(sbddextended_MyVector));
    if (node_index->level_vec_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (i = 1; i <= node_index->height; ++i) {
        sbddextended_MyDict_initialize(&node_index->node_dict_arr[i]);
        sbddextended_MyVector_initialize(&node_index->level_vec_arr[i]);
    }

    sbddextended_MyDict_add(&node_index->node_dict_arr[node_index->height],
                            (llint)f,
                            0ll); /* 0 means the first node in the level */
    sbddextended_MyVector_add(&node_index->level_vec_arr[node_index->height], (llint)f);

    for (i = node_index->height; i >= 1; --i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_raw) {
                    if (is_zbdd) {
                        child = bddgetchildzraw(node, k);
                    } else {
                        child = bddgetchildbraw(node, k);
                    }
                    child = bdderasenot(child);
                } else {
                    if (is_zbdd) {
                        child = bddgetchildz(node, k);
                    } else {
                        child = bddgetchildb(node, k);
                    }
                }

                if (!bddisterminal(child)) {
                    level = (int)bddgetlev(child);
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[level],
                                                 (llint)child, NULL) == 0) {
                        sbddextended_MyDict_add(&node_index->node_dict_arr[level],
                                                (llint)child,
                                                (llint)node_index->node_dict_arr[level].count);
                        sbddextended_MyVector_add(&node_index->level_vec_arr[level], (llint)child);
                    }
                }
            }
        }
    }

    node_index->offset_arr = (llint*)malloc((size_t)(node_index->height + 1) * sizeof(llint));
    if (node_index->offset_arr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    node_index->offset_arr[node_index->height] = sbddextended_BDDNODE_START;

    for (i = node_index->height; i >= 1; --i) {
        node_index->offset_arr[i - 1] = node_index->offset_arr[i] + (llint)node_index->level_vec_arr[i].count;
    }

    node_index->count_arr = NULL;
    return node_index;
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
bddNodeIndex* bddNodeIndex_makeIndex_inner(bddp f, int is_raw, int is_zbdd)
{
    int i, clevel, raw_flag;
    llint j, id0, id1;
    bddp node, n0, n1;
    bddNodeIndex* node_index;

    node_index = bddNodeIndex_makeIndexWithoutCount_inner(f, is_raw, is_zbdd);

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        return node_index;
    }

    if (is_zbdd) {
        node_index->count_arr = (ullint*)malloc((size_t)node_index->offset_arr[0] * sizeof(ullint));
        if (node_index->count_arr == NULL) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
        node_index->count_arr[0] = 0;
        node_index->count_arr[1] = 1;

        for (i = 1; i <= node_index->height; ++i) {
            for (j = node_index->offset_arr[i]; j < node_index->offset_arr[i - 1]; ++j) {
                node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i],
                                                       j - node_index->offset_arr[i]);
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
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                     (llint)n0, &id0) == 0) {
                        fprintf(stderr, "node not found!\n");
                        exit(1);
                    }
                    id0 += node_index->offset_arr[clevel];
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
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                     (llint)n1, &id1) == 0) {
                        fprintf(stderr, "node not found!\n");
                        exit(1);
                    }
                    id1 += node_index->offset_arr[clevel];
                }
                /* We do not check the overflow. */
                node_index->count_arr[j] = node_index->count_arr[id0] + node_index->count_arr[id1];
                if (is_raw && raw_flag) {
                    node_index->count_arr[j] += 1;
                }
            }
        }
    } else {
        /* not implemented yet. */
    }
    return node_index;
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
ullint bddNodeIndex_size(const bddNodeIndex* node_index)
{
    if (node_index->f == bddnull || node_index->f == bddfalse || node_index->f == bddtrue) {
        return 0ull;
    }
    return (ullint)node_index->offset_arr[0] - sbddextended_BDDNODE_START;
}

sbddextended_INLINE_FUNC
ullint bddNodeIndex_sizeAtLevel(const bddNodeIndex* node_index, int level)
{
    if (node_index->f == bddnull || node_index->f == bddfalse || node_index->f == bddtrue) {
        return 0ull;
    } else if (level <= 0 || node_index->height < level) {
        return 0ull;
    }
    assert(node_index->offset_arr[level - 1] >= node_index->offset_arr[level]);
    return (ullint)(node_index->offset_arr[level - 1] - node_index->offset_arr[level]);
}

sbddextended_INLINE_FUNC
void bddNodeIndex_sizeEachLevel(const bddNodeIndex* node_index, bddvar* arr)
{
    int i;
    if (!(node_index->f == bddnull || bddisterminal(node_index->f))) {
        for (i = 1; i <= node_index->height; ++i) {
            assert(node_index->offset_arr[i - 1] >= node_index->offset_arr[i]);
            arr[i] = (bddvar)(node_index->offset_arr[i - 1] - node_index->offset_arr[i]);
        }
    }
}

sbddextended_INLINE_FUNC
ullint bddNodeIndex_count(const bddNodeIndex* node_index)
{
    /* We assume that the two values are the same. */
    assert(bddtrue == bddsingle);
    if (node_index->f == bddnull || node_index->f == bddfalse
            || node_index->f == bddempty) {
        return 0ull;
    } else if (node_index->f == bddtrue) {
        return 1ull;
    }
    if (node_index->is_raw) {
        if (bddisnegative(node_index->f)) {
            return node_index->count_arr[sbddextended_BDDNODE_START] + 1;
        } else {
            return node_index->count_arr[sbddextended_BDDNODE_START];
        }
    } else {
        return node_index->count_arr[sbddextended_BDDNODE_START];
    }
}

sbddextended_INLINE_FUNC
void bddNodeIndex_destruct(bddNodeIndex* node_index)
{
    int i;

    if (node_index != NULL) {
        if (node_index->level_vec_arr != NULL) {
            for (i = 1; i <= node_index->height; ++i) {
                sbddextended_MyVector_deinitialize(&node_index->level_vec_arr[i]);
            }
            free(node_index->level_vec_arr);
        }
        if (node_index->node_dict_arr != NULL) {
            for (i = 1; i <= node_index->height; ++i) {
                sbddextended_MyDict_deinitialize(&node_index->node_dict_arr[i]);
            }
            free(node_index->node_dict_arr);
        }
        if (node_index->offset_arr != NULL) {
            free(node_index->offset_arr);
        }
        if (node_index->count_arr != NULL) {
            free(node_index->count_arr);
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


/* *************************** C++ version start ***************************** */

#ifdef __cplusplus

/* This class is obsolate. */
class DDNodeIndex {
private:
    bddNodeIndex* node_index_;

public:
    DDNodeIndex(const BDD& f, bool is_raw = true)
    {
        node_index_ = bddNodeIndex_makeIndex_inner(f.GetID(), (is_raw ? 1 : 0), 0);
    }

    DDNodeIndex(const ZBDD& f, bool is_raw = true)
    {
        node_index_ = bddNodeIndex_makeIndex_inner(f.GetID(), (is_raw ? 1 : 0), 1);
    }

    bddNodeIndex* getRawPointer()
    {
        return node_index_;
    }

    ullint size()
    {
        return bddNodeIndex_size(node_index_);
    }

    ullint sizeAtLevel(int level)
    {
        return bddNodeIndex_sizeAtLevel(node_index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr)
    {
        if (!(node_index_->f == bddnull || node_index_->f == bddfalse || node_index_->f == bddtrue)) {
            arr.resize(node_index_->height + 1);
            for (int i = 1; i <= node_index_->height; ++i) {
                arr[i] = (bddvar)(node_index_->offset_arr[i - 1] - node_index_->offset_arr[i]);
            }
        }
    }

    ullint count()
    {
        return bddNodeIndex_count(node_index_);
    }

    ~DDNodeIndex()
    {
        bddNodeIndex_destruct(node_index_);
        free(node_index_);
    }

#if __cplusplus >= 201703L
    class DDNodeIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = bddp;
        using difference_type = std::ptrdiff_t;
        using pointer = bddp*;
        using reference = bddp&;
    private:
#else
    class DDNodeIterator : public std::iterator<std::input_iterator_tag, bddp>
    {
    private:
#endif
        DDNodeIndex* node_index_;
        size_t pos_;
        size_t level_;

    public:
        DDNodeIterator(DDNodeIndex* node_index, bool is_end) : node_index_(node_index), pos_(0)
        {
            if (is_end) {
                level_ = 0; /* This means pointing at end; */
            } else {
                level_ = node_index->node_index_->height;
            }
        }

        bddp operator*() const
        {
            if (level_ <= 0) {
                return bddfalse;
            }
            return (bddp)sbddextended_MyVector_get(&node_index_->node_index_->
                                                    level_vec_arr[level_],
                                                    (llint)pos_);
        }

        DDNodeIterator& operator++()
        {
            if (level_ > 0) {
                ++pos_;
                while (level_ > 0 &&
                       pos_ >= node_index_->node_index_->level_vec_arr[level_].count) {
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
        return DDNodeIterator(this, false);
    }

    DDNodeIterator end()
    {
        return DDNodeIterator(this, true);
    }
};

template <typename T> class DDIndex;

template <typename T>
class DDNode {
private:
    const bddp f_;
    DDIndex<T>& node_index_;
public:
    T& value;

public:
    DDNode(bddp f, DDIndex<T>& node_index) : f_(f), node_index_(node_index), value(node_index.getStorageRef(f)) { }

    bddp getBddp() const
    {
        return f_;
    }

    DDNode<T> child(int c) const
    {
        bddp g;
        if (isTerminal()) {
            g = f_;
        } else if (bddisbdd(f_)) {
            g = bddgetchildb(f_, c);
        } else {
            g = bddgetchildz(f_, c);
        }
        DDNode<T> node(g, node_index_);
        return node;
    }

    bool isTerminal() const
    {
        return (!!bddisterminal(f_));
    }

    bool isTerminal(int t) const
    {
        if (t == 0 && f_ == bddfalse) {
            return true;
        } else if (t == 1 && f_ == bddtrue) {
            return true;
        } else {
            return false;
        }
    }
};

template <typename T>
bool operator==(const DDNode<T>& node1, const DDNode<T>& node2)
{
    return node1.getBddp() == node2.getBddp();
}

template <typename T>
bool operator!=(const DDNode<T>& node1, const DDNode<T>& node2)
{
    return node1.getBddp() != node2.getBddp();
}

template <typename T>
class DDIndex {
private:
    bddNodeIndex* node_index_;
    std::map<bddp, T> storage_;
    bool is_count_made;

#ifdef SBDDH_GMP
    typedef mpz_class count_t;
#else
    typedef ullint count_t;
#endif
    typedef std::map<bddp, count_t> map_t;
    map_t count_storage_;

    void initialize(bddp f, bool /*is_raw*/, int is_zbdd)
    {
        /* currently, we do not support raw mode. We set is_raw to be false. */
        /*node_index_ = bddNodeIndex_makeIndexWithoutCount_inner(f, (is_raw ? 1 : 0), is_zbdd); */
        node_index_ = bddNodeIndex_makeIndexWithoutCount_inner(f, 0, is_zbdd);
    }

    llint optimize(const std::vector<llint>& weights, bool is_max,
                    std::set<bddvar>& s) const
    {
        if (node_index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        /* llint -> max/min value, bool -> 1-arc if true, 0-arc if false */
        std::map<bddp, std::pair<llint, bool> > sto;

        if (is_max) {
            sto[bddempty].first = LLONG_MIN;
        } else {
            sto[bddempty].first = LLONG_MAX;
        }
        sto[bddsingle].first = 0;

        for (int level = 1; level <= height(); ++level) {
            for (ullint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                if (is_max) {
                    if (sto[child1].first == sto[bddempty].first
                            || sto[child0].first > sto[child1].first + weights[var]) {
                        sto[f].first = sto[child0].first;
                        sto[f].second = false; /* 0-arc side */
                    } else {
                        sto[f].first = sto[child1].first + weights[var];
                        sto[f].second = true; /* 1-arc side */
                    }
                } else {
                    if (sto[child1].first == sto[bddempty].first
                            || sto[child0].first > sto[child1].first + weights[var]) {
                        sto[f].first = sto[child1].first + weights[var];
                        sto[f].second = true; /* 1-arc side */
                    } else {
                        sto[f].first = sto[child0].first;
                        sto[f].second = false; /* 0-arc side */
                    }
                }
            }
        }
        bddp g = node_index_->f;
        while (!bddisterminal(g)) {
            if (sto[g].second) { /* 1-arc */
                s.insert(bddgetvar(g));
                g = bddgetchild1z(g);
            } else { /* 0-arc */
                g = bddgetchild0z(g);
            }
        }
        assert(g == bddsingle);
        return sto[node_index_->f].first;
    }

#ifdef SBDDH_GMP
    ullint getStorageValue(bddp f) const
    {
        if (count_storage_.count(f) > 0) {
            return sbddh_mpz_to_ullint(count_storage_.at(f));
        } else {
            bddp fn = bddtakenot(f);
            std::map<bddp, mpz_class>::const_iterator itor = count_storage_.find(fn);
            if (itor == count_storage_.end()) {
                std::cerr << "key f not found" << std::endl;
                exit(1);
            } else {
                return sbddh_mpz_to_ullint(itor->second) + (bddisnegative(f) ? 1 : -1);
            }
        }
    }

#else

    ullint getStorageValue(bddp f) const
    {
        if (count_storage_.count(f) > 0) {
            return count_storage_.at(f);
        } else {
            bddp fn = bddtakenot(f);
            std::map<bddp, ullint>::const_iterator itor = count_storage_.find(fn);
            if (itor == count_storage_.end()) {
                std::cerr << "key f not found" << std::endl;
                exit(1);
            } else {
                if (bddisnegative(f)) {
                    return itor->second + 1;
                } else {
                    assert(itor->second > 0);
                    return itor->second - 1;
                }
            }
        }
    }

#endif

    template<typename value_t>
    value_t getStorageValue2(bddp f) const
    {
        if (count_storage_.count(f) > 0) {
            return sbddh_getValueFromMpz<value_t>(count_storage_.at(f));
        } else {
            bddp fn = bddtakenot(f);
            map_t::const_iterator itor = count_storage_.find(fn);
            if (itor == count_storage_.end()) {
                std::cerr << "key f not found" << std::endl;
                exit(1);
            } else {
                if (bddisnegative(f)) {
                    return sbddh_getValueFromMpz<value_t>(itor->second)
                        + sbddh_getOne<value_t>();
                } else {
                    assert(itor->second > 0);
                    return sbddh_getValueFromMpz<value_t>(itor->second)
                        - sbddh_getOne<value_t>();
                }
            }
        }
    }

    llint getOrderNumber(bddp f, std::set<bddvar>& s) const
    {
        bddp f0;

        if (f == bddempty) {
            return -1;
        } else if (f == bddsingle) {
            if (s.size() > 0) {
                return -1;
            } else {
                return 0;
            }
        }

        if (s.size() == 0) {
            if (bddisemptymember(f)) {
                return 0;
            } else {
                return -1;
            }
        }

        llint value = 0;
        if (bddisemptymember(f)) {
            value += 1;
            f0 = bdderasenot(f);
        } else {
            f0 = f;
        }

        bddvar var = bddgetvar(f);
        if (s.count(var) >= 1) {
            s.erase(var);
            llint v = getOrderNumber(bddgetchild1z(f), s);
            if (v >= 0) {
                value += v;
            } else {
                value = -1;
            }
        } else {
            llint v = getStorageValue(bddgetchild1z(f))
                        + getOrderNumber(bddgetchild0z(f0), s);
            if (v >= 0) {
                value += v;
            } else {
                value = -1;
            }
        }
        return value;
    }

#ifdef SBDDH_GMP
    mpz_class getOrderNumberMP(bddp f, std::set<bddvar>& s) const
    {
        bddp f0;

        if (f == bddempty) {
            return mpz_class(-1);
        } else if (f == bddsingle) {
            if (s.size() > 0) {
                return mpz_class(-1);
            } else {
                return mpz_class(0);
            }
        }

        if (s.size() == 0) {
            if (bddisemptymember(f)) {
                return mpz_class(0);
            } else {
                return mpz_class(-1);
            }
        }

        mpz_class value(0);
        if (bddisemptymember(f)) {
            value += mpz_class(1);
            f0 = bdderasenot(f);
        } else {
            f0 = f;
        }

        bddvar var = bddgetvar(f);
        if (s.count(var) >= 1) {
            s.erase(var);
            mpz_class v = getOrderNumberMP(bddgetchild1z(f), s);
            if (v >= 0) {
                value += v;
            } else {
                value = mpz_class(-1);
            }
        } else {
            mpz_class v = count_storage_.at(bddgetchild1z(f))
                + getOrderNumberMP(bddgetchild0z(f0), s);
            if (v >= 0) {
                value += v;
            } else {
                value = mpz_class(-1);
            }
        }
        return value;
    }
#endif

    void getSet(bddp f, llint order, std::set<bddvar>& s)
    {
        bddp f0 = f;

        if (f == bddempty || f == bddsingle) {
            return;
        }

        if (bddisemptymember(f)) {
            if (order == 0) {
                return;
            } else {
                order -= 1;
                f0 = bdderasenot(f);
            }
        }

        llint card1 = static_cast<llint>(getStorageValue(bddgetchild1z(f)));
        if (order < card1) {
            s.insert(bddgetvar(f));
            getSet(bddgetchild1z(f), order, s);
        } else {
            getSet(bddgetchild0z(f0), order - card1, s);
        }
    }

#ifdef SBDDH_GMP
    void getSetMP(bddp f, mpz_class order, std::set<bddvar>& s)
    {
        bddp f0 = f;

        if (f == bddempty || f == bddsingle) {
            return;
        }

        if (bddisemptymember(f)) {
            if (order == mpz_class(0)) {
                return;
            } else {
                order -= mpz_class(1);
                f0 = bdderasenot(f);
            }
        }

        mpz_class card1 = count_storage_[bddgetchild1z(f)];
        if (order < card1) {
            s.insert(bddgetvar(f));
            getSetMP(bddgetchild1z(f), order, s);
        } else {
            getSetMP(bddgetchild0z(f0), order - card1, s);
        }
    }
#endif

    template<typename value_t>
    bddp getKSetsZBDD(bddp f, value_t k)
    {
        if (k <= sbddh_getZero<value_t>() || f == bddempty) {
            return bddempty;
        } else if (f == bddsingle) {
            return bddsingle;
        } else if (k >= getStorageValue2<value_t>(f)) {
            return bddcopy(f);
        } else {
            bddp fn = f;
            bddp f1 = bddgetchild1z(f);
            bddp g;
            value_t card1 = getStorageValue2<value_t>(f1);
            if (bddisemptymember(f)) {
                if (k == sbddh_getOne<value_t>()) {
                    return bddsingle;
                }
                card1 += sbddh_getOne<value_t>();
                fn = bdderasenot(f);
            }
            if (k > card1) {
                bddp f0 = bddgetchild0z(fn);
                bddp g0 = getKSetsZBDD<value_t>(f0, k - card1);
                if (bddisemptymember(f)) {
                    assert(!bddisnegative(g0));
                    g0 = bddtakenot(g0);
                }
                g = bddmakenodez(bddgetvar(f), g0, f1);
                bddfree(g0);
            } else {
                bddp g1;
                if (bddisemptymember(f)) {
                    g1 = getKSetsZBDD<value_t>(f1, k - sbddh_getOne<value_t>());
                } else {
                    g1 = getKSetsZBDD<value_t>(f1, k);
                }

                if (bddisemptymember(f)) {
                    g = bddmakenodez(bddgetvar(f), bddsingle, g1);
                } else {
                    g = bddmakenodez(bddgetvar(f), bddempty, g1);
                }
                bddfree(g1);
            }
#ifdef SBDDH_GMP
            assert(sbddh_getValueFromMpz<value_t>(
                sbddh_ullint_to_mpz(static_cast<ullint>(bddcard(g)))) == k);
#else
            assert(static_cast<ullint>(bddcard(g)) == k);
#endif
            return g;
        }
    }

#ifdef SBDDH_BDDCT
    template<typename value_t>
    ZBDD getKLightestZBDD(const ZBDD& f, const value_t& k,
        const std::vector<llint>& weights, int strict)
    {
        if (k <= sbddh_getZero<value_t>() || f == ZBDD(0)) {
            return ZBDD(0);
        }
        makeCountIndex();
        /* k is more than or equal to the card of f */
        if (k >= getStorageValue2<value_t>(f.GetID())) {
            return f;
        }
        BDDCT bddct;
        const int lev = getLev(f);
        if (!weightRange_initialize(&bddct, lev, weights)) {
            return ZBDD(-1);
        }

        /* binary search: k is in (left_bound, right_bound) */
        int left_bound = bddct.MinCost(f) - 1;
        int right_bound = bddct.MaxCost(f);
        assert(left_bound < right_bound);
        ZBDD c_zbdd;
        ZBDD left_zbdd(0);
        ZBDD right_zbdd = f;
        value_t c_card;
        value_t left_card = 0;
        while (right_bound - left_bound > 1) {
            int c_bound = left_bound + (right_bound - left_bound) / 2;
            assert(c_bound >= left_bound + 1);
            c_zbdd = bddct.ZBDD_CostLE(f, c_bound);
            c_card = sbddh_getCard<value_t>(c_zbdd);
            if (c_card == k) {
                return c_zbdd;
            } else if (c_card < k) {
                left_bound = c_bound;
                left_zbdd = c_zbdd;
                left_card = c_card;
            } else {
                right_bound = c_bound;
                right_zbdd = c_zbdd;
            }
            assert(left_bound < right_bound);
        }
        assert(left_bound + 1 == right_bound);
        /* assert(bddct.ZBDD_CostLE(f, left_bound) == left_zbdd); */
        /* assert(bddct.ZBDD_CostLE(f, right_bound) == right_zbdd); */
        if (strict < 0) {
            return left_zbdd;
        } else if (strict > 0) {
            return right_zbdd;
        } else {
            ZBDD delta = right_zbdd - left_zbdd;
            DDIndex<int> delta_index(delta);
            return left_zbdd + delta_index.getKSetsZBDD(k - left_card);
        }
    }
#endif /* SBDDH_BDDCT */

    template<typename value_t>
    void sampleRandomlyA(ullint* rand_state, std::set<bddvar>& s)
    {
        bddp f = node_index_->f;
        while (!bddisconstant(f)) {
            bddp f0 = bddgetchild0(f);
            bddp f1 = bddgetchild1(f);
            value_t card0 = getStorageValue2<value_t>(f0);
            value_t card1 = getStorageValue2<value_t>(f1);
            double r = static_cast<double>(sbddextended_getXRand(rand_state) - 1)
                    /* / 0xffffffffffffffffull; */
                    / 1.8446744073709552e+19; /* avoid warning */
            if (r < sbddh_divide<value_t>(card0, card0 + card1)) {
                f = f0;
            } else {
                s.insert(bddgetvar(f));
                f = f1;
            }
        }
        assert(f == bddsingle);
    }

    bddp getBddp(int level, ullint pos) const
    {
        return static_cast<bddp>(sbddextended_MyVector_get(&node_index_->
                                                    level_vec_arr[level],
                                                    static_cast<llint>(pos)));
    }

public:
    DDIndex(const BDD& f, bool is_raw = false) : is_count_made(false)
    {
        initialize(f.GetID(), is_raw, 0);
    }

    DDIndex(const ZBDD& f, bool is_raw = false) : is_count_made(false)
    {
        initialize(f.GetID(), is_raw, 1);
    }

    DDIndex(bddp f, bool is_raw = false) : is_count_made(false)
    {
        initialize(f, is_raw, (bddiszbdd(f) ? 1 : 0));
    }

    ~DDIndex()
    {
        bddNodeIndex_destruct(node_index_);
        free(node_index_);
    }

    bddNodeIndex* getRawPointer()
    {
        return node_index_;
    }

    T& getStorageRef(bddp f)
    {
        return storage_[f];
    }

    ZBDD getZBDD() const
    {
        return ZBDD_ID(bddcopy(node_index_->f));
    }

    int height() const
    {
        return bddgetlev(node_index_->f);
    }

    ullint size() const
    {
        return bddNodeIndex_size(node_index_);
    }

    ullint size(int level) const
    {
        return bddNodeIndex_sizeAtLevel(node_index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr) const
    {
        if (!(node_index_->f == bddnull || node_index_->f == bddfalse || node_index_->f == bddtrue)) {
            arr.resize(node_index_->height + 1);
            for (int i = 1; i <= node_index_->height; ++i) {
                arr[i] = (bddvar)(node_index_->offset_arr[i - 1] - node_index_->offset_arr[i]);
            }
        }
    }

    std::set<bddvar> usedVar() const
    {
        std::set<bddvar> result;
        std::vector<bddvar> size_arr;
        sizeEachLevel(size_arr);
        for (int lev = 1; lev <= node_index_->height; ++lev) {
            if (size_arr[lev] > 0) {
                result.insert(bddvaroflev(lev));
            }
        }
        return result;
    }

    ullint count()
    {
        makeCountIndex();
        return getStorageValue2<ullint>(node_index_->f);
    }

#ifdef SBDDH_GMP
    mpz_class countMP()
    {
        makeCountIndex();
        return getStorageValue2<mpz_class>(node_index_->f);
    }

    mpz_class count_v()
    {
        return countMP();
    }
#else
    ullint count_v()
    {
        return count();
    }
#endif

    llint getMaximum(const std::vector<llint>& weights, std::set<bddvar>& s) const
    {
        if (node_index_->f == bddempty) {
            return 0ll;
        }
        return optimize(weights, true, s);
    }

    llint getMaximum(const std::vector<llint>& weights) const
    {
        if (node_index_->f == bddempty) {
            return 0ll;
        }
        std::set<bddvar> dummy;
        return optimize(weights, true, dummy);
    }

    llint getMinimum(const std::vector<llint>& weights, std::set<bddvar>& s) const
    {
        if (node_index_->f == bddempty) {
            return 0ll;
        }
        return optimize(weights, false, s);
    }

    llint getMinimum(const std::vector<llint>& weights) const
    {
        if (node_index_->f == bddempty) {
            return 0ll;
        }
        std::set<bddvar> dummy;
        return optimize(weights, false, dummy);
    }

    llint getSum(const std::vector<llint>& weights)
    {
        if (node_index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        makeCountIndex();

        std::map<bddp, llint> sto;

        sto[bddempty] = 0;
        sto[bddsingle] = 0;

        for (int level = 1; level <= height(); ++level) {
            for (ullint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                sto[f] = sto[child0] + sto[child1] + weights[var] * getStorageValue(child1);
            }
        }
        return sto[node_index_->f];
    }

#ifdef SBDDH_GMP
    mpz_class getSumMP(const std::vector<llint>& weights)
    {
        if (node_index_->is_raw) {
            std::cerr << "DDIndex currently does not support raw mode." << std::endl;
            exit(1);
        }

        makeCountIndex();

        std::map<bddp, mpz_class> sto;

        sto[bddempty] = mpz_class(0);
        sto[bddsingle] = mpz_class(0);

        for (int level = 1; level <= height(); ++level) {
            for (ullint pos = 0; pos < size(level); ++pos) {
                int var = bddvaroflev(level);
                bddp f = getBddp(level, pos);
                bddp child0 = bddgetchild0z(f);
                bddp child1 = bddgetchild1z(f);
                mpz_class w_mp = sbddh_llint_to_mpz(weights[var]);
                sto[f] = sto[child0] + sto[child1] + w_mp * count_storage_.at(child1);
            }
        }
        return sto[node_index_->f];
    }
#endif

    llint getOrderNumber(const std::set<bddvar>& s)
    {
        makeCountIndex();
        std::set<bddvar> ss(s);
        return getOrderNumber(node_index_->f, ss);
    }

#ifdef SBDDH_GMP
    mpz_class getOrderNumberMP(const std::set<bddvar>& s)
    {
        makeCountIndex();
        std::set<bddvar> ss(s);
        return getOrderNumberMP(node_index_->f, ss);
    }
#endif

    std::set<bddvar> getSet(llint order)
    {
        makeCountIndex();
        if (order < 0 || order >= static_cast<llint>(count())) { /* out of range */
            return std::set<bddvar>();
        }
        std::set<bddvar> s;
        getSet(node_index_->f, order, s);
        return s;
    }

#ifdef SBDDH_GMP
    std::set<bddvar> getSet(mpz_class order)
    {
        makeCountIndex();
        if (order < mpz_class(0) || order >= countMP()) { /* out of range */
            return std::set<bddvar>();
        }
        std::set<bddvar> s;
        getSetMP(node_index_->f, order, s);
        return s;
    }
#endif

    ZBDD getKSetsZBDD(ullint k)
    {
        if (k <= 0) {
            return ZBDD(0);
        }
        makeCountIndex();
        return ZBDD_ID(getKSetsZBDD<ullint>(node_index_->f, k));
    }

#ifdef SBDDH_GMP
    ZBDD getKSetsZBDD(const mpz_class& k)
    {
        if (k <= 0) {
            return ZBDD(0);
        }
        makeCountIndex();
        return ZBDD_ID(getKSetsZBDD<mpz_class>(node_index_->f, k));
    }
#endif

#ifdef SBDDH_BDDCT
    ZBDD getKLightestZBDD(ullint k,
        const std::vector<llint>& weights, int strict)
    {
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return getKLightestZBDD<ullint>(f, k, weights, strict);
    }

#ifdef SBDDH_GMP
    ZBDD getKLightestZBDD(const mpz_class& k,
        const std::vector<llint>& weights, int strict)
    {
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return getKLightestZBDD<mpz_class>(f, k, weights, strict);
    }
#endif /* SBDDH_GMP */

    ZBDD getKHeaviestZBDD(ullint k,
        const std::vector<llint>& weights, int strict)
    {
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return f - getKLightestZBDD(count() - k, weights, -strict);
    }

#ifdef SBDDH_GMP
    ZBDD getKHeaviestZBDD(const mpz_class& k,
        const std::vector<llint>& weights, int strict)
    {
        ZBDD f = ZBDD_ID(bddcopy(node_index_->f));
        return f - getKLightestZBDD(countMP() - k, weights, -strict);
    }
#endif /* SBDDH_GMP */

#endif /* SBDDH_BDDCT */

#ifdef SBDDH_GMP /* use GMP random */
    std::set<bddvar> sampleRandomly(gmp_randclass& random)
    {
        makeCountIndex();
        return getSet(random.get_z_range(countMP()));
    }
#else /* SBDDH_GMP */

#if __cplusplus >= 201103L /* use C++ random class */

    template <typename U>
    std::set<bddvar> sampleRandomly(U& random_engine)
    {
        std::uniform_int_distribution<llint> dist(0, count() - 1);
        return getSet(dist(random_engine));
    }

#else /* __cplusplus >= 201103L // use rand() function */

    std::set<bddvar> sampleRandomly()
    {
        makeCountIndex();
        if (count() >= 1) {
            return getSet(rand() % count());
        } else {
            return std::set<bddvar>();
        }
    }

#endif /* __cplusplus >= 201103L */

#endif /* SBDDH_GMP */

    std::set<bddvar> sampleRandomlyA(ullint* rand_state)
    {
        makeCountIndex();
        if (count() >= 1) {
            std::set<bddvar> s;
#ifdef SBDDH_GMP
            /* card is larger than or equal to 2^64 */
            if (countMP() >= mpz_class("18446744073709551616")) {
                sampleRandomlyA<mpz_class>(rand_state, s);
                return s;
            }
#endif
            sampleRandomlyA<ullint>(rand_state, s);
            return s;
        } else {
            return std::set<bddvar>();
        }
    }

    DDNode<T> root()
    {
        return DDNode<T>(node_index_->f, *this);
    }

    DDNode<T> terminal(int t)
    {
        return DDNode<T>((t != 0 ? bddtrue : bddfalse), *this);
    }

    DDNode<T> getNode(int level, llint pos)
    {
        return DDNode<T>(getBddp(level, pos), *this);
    }

    void makeCountIndex() /* currently support only for ZDD */
    {
        if (!is_count_made) {
            is_count_made = true;

            if (node_index_->is_raw) {
                std::cerr << "DDIndex currently does not support raw mode." << std::endl;
                exit(1);
            }
            count_storage_[bddempty] = sbddextended_VALUE_ZERO;
            count_storage_[bddsingle] = sbddextended_VALUE_ONE;
            for (int level = 1; level <= height(); ++level) {
                for (ullint pos = 0; pos < size(level); ++pos) {
                    bddp f = getBddp(level, pos);
                    bddp child0 = bddgetchild0z(f);
                    bddp child1 = bddgetchild1z(f);
                    /* We do not check the overflow when count_t is ullint */
                    count_storage_[f] = count_storage_[child0] + count_storage_[child1];
                }
            }
        }
    }

#if __cplusplus >= 201703L
    class DDNodeIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = bddp;
        using difference_type = std::ptrdiff_t;
        using pointer = bddp*;
        using reference = bddp&;
    private:
#else
    class DDNodeIterator : public std::iterator<std::input_iterator_tag, bddp>
    {
    private:
#endif
    private:
        DDIndex* node_index_;
        size_t pos_;
        size_t level_;

    public:
        DDNodeIterator(DDIndex* node_index, bool is_end) : node_index_(node_index), pos_(0)
        {
            if (is_end) {
                level_ = 0; /* This means pointing at end; */
            } else {
                level_ = node_index->node_index_->height;
            }
        }

        bddp operator*() const
        {
            if (level_ <= 0) {
                return bddfalse;
            }
            return sbddextended_MyVector_get(&node_index_->node_index_->
                                                level_vec_arr[level_],
                                                (llint)pos_);
        }

        DDNodeIterator& operator++()
        {
            if (level_ > 0) {
                ++pos_;
                while (level_ > 0 &&
                        pos_ >= node_index_->node_index_->level_vec_arr[level_].count) {
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

#if __cplusplus >= 201703L
    class WeightIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::set<bddvar>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::set<bddvar>*;
        using reference = std::set<bddvar>&;
    private:
#else
    class WeightIterator : public std::iterator<std::input_iterator_tag, std::set<bddvar> >
    {
    private:
#endif
        ZBDD f_;
        std::set<bddvar> current_;
        std::vector<llint> weights_;
        bool is_min_;

        void setCurrent()
        {
            current_.clear();
            DDIndex<int> current_index(f_);
            if (is_min_) {
                current_index.getMinimum(weights_, current_);
            } else {
                current_index.getMaximum(weights_, current_);
            }
        }

    public:
        WeightIterator() : f_(ZBDD(0)), is_min_(false) { }

        WeightIterator(const ZBDD& f,
                const std::vector<llint>& weights,
                bool is_min) :
                    f_(f), weights_(weights), is_min_(is_min)
        {
            setCurrent();
        }

        std::set<bddvar> operator*() const
        {
            return current_;
        }

        WeightIterator& operator++()
        {
            f_ -= getSingleSet(current_);
            setCurrent();
            return *this;
        }

        bool operator==(const WeightIterator& it) const
        {
            return f_ == it.f_;
        }

        bool operator!=(const WeightIterator& it) const
        {
            return !(operator==(it));
        }
    };

#if __cplusplus >= 201703L
    class RandomIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::set<bddvar>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::set<bddvar>*;
        using reference = std::set<bddvar>&;
    private:
#else
    class RandomIterator : public std::iterator<std::input_iterator_tag, std::set<bddvar> >
    {
    private:
#endif
        ZBDD f_;
        std::set<bddvar> current_;
        ullint rand_seed_;

        void setCurrent()
        {
            DDIndex<int> current_index(f_);
            current_ = current_index.sampleRandomlyA(&rand_seed_);
        }

    public:
        RandomIterator() : f_(ZBDD(0)),
                rand_seed_(0) { }

        RandomIterator(const ZBDD& f,
                ullint rand_seed) : f_(f),
                rand_seed_(rand_seed)
        {
            setCurrent();
        }

        std::set<bddvar> operator*() const
        {
            return current_;
        }

        RandomIterator& operator++()
        {
            f_ -= getSingleSet(current_);
            setCurrent();
            return *this;
        }

        bool operator==(const RandomIterator& it) const
        {
            return f_ == it.f_;
        }

        bool operator!=(const RandomIterator& it) const
        {
            return !(operator==(it));
        }
    };

#if __cplusplus >= 201703L
    class DictIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::set<bddvar>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::set<bddvar>*;
        using reference = std::set<bddvar>&;
    private:
#else
    class DictIterator : public std::iterator<std::input_iterator_tag, std::set<bddvar> >
    {
    private:
#endif
        DDIndex<T>* dd_index_;
        count_t card_;
        bool reverse_;
        count_t current_;

    public:
        DictIterator(count_t current) :
            dd_index_(NULL),
            card_(0),
            reverse_(false),
            current_(current)
        { }

        DictIterator(DDIndex<T>* dd_index, bool reverse) :
            dd_index_(dd_index),
            card_(dd_index->count_v()),
            reverse_(reverse),
            current_(reverse ? card_ : 0)
        { }

        std::set<bddvar> operator*() const
        {
            if (reverse_) {
                return dd_index_->getSet(current_ - 1);
            } else {
                return dd_index_->getSet(current_);
            }
        }

        DictIterator& operator++()
        {
            if (reverse_) {
                --current_;
            } else {
                ++current_;
            }
            return *this;
        }

        bool operator==(const DictIterator& it) const
        {
            return current_ == it.current_;
        }

        bool operator!=(const DictIterator& it) const
        {
            return !(operator==(it));
        }
    };

    DDNodeIterator begin() const
    {
        return DDNodeIterator(this, false);
    }

    DDNodeIterator end() const
    {
        return DDNodeIterator(this, true);
    }

    WeightIterator weight_min_begin(const std::vector<llint>& weights) const
    {
        return WeightIterator(ZBDD_ID(bddcopy(node_index_->f)),
            weights, true);
    }

    WeightIterator weight_min_end() const
    {
        return WeightIterator();
    }

    WeightIterator weight_max_begin(const std::vector<llint>& weights) const
    {
        return WeightIterator(ZBDD_ID(bddcopy(node_index_->f)),
            weights, false);
    }

    WeightIterator weight_max_end() const
    {
        return WeightIterator();
    }

    RandomIterator random_begin(ullint rand_seed = 1) const
    {
        return RandomIterator(ZBDD_ID(bddcopy(node_index_->f)), rand_seed);
    }

    RandomIterator random_end() const
    {
        return RandomIterator();
    }

    DictIterator dict_begin()
    {
        return DictIterator(this, false);
    }

    DictIterator dict_end()
    {
        return DictIterator(count_v());
    }

    DictIterator dict_rbegin()
    {
        return DictIterator(this, true);
    }

    DictIterator dict_rend()
    {
        return DictIterator(0);
    }
};

#ifdef SBDDH_GMP

template<>
inline
mpz_class sbddh_getCard<mpz_class>(const ZBDD& f)
{
    DDIndex<int> dd_index(f);
    return dd_index.countMP();
}

#endif /* SBDDH_GMP */

template<>
inline
ullint sbddh_getCard<ullint>(const ZBDD& f)
{
    return static_cast<ullint>(f.Card());
}

#endif /* __cplusplus */


typedef struct tagbddNodeIterator {
    bddNodeIndex* node_index;
    size_t pos;
    llint level;
} bddNodeIterator;

sbddextended_INLINE_FUNC
bddNodeIterator* bddNodeIterator_make(bddNodeIndex* node_index)
{
    bddNodeIterator* itor;

    itor = (bddNodeIterator*)malloc(sizeof(bddNodeIterator));
    if (itor == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    itor->node_index = node_index;
    itor->pos = 0;
    itor->level = itor->node_index->height;
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

    f = (bddp)sbddextended_MyVector_get(&itor->node_index->level_vec_arr[itor->level],
                                        (llint)itor->pos);
    ++itor->pos;

    while (itor->level > 0 && itor->pos >= itor->node_index->level_vec_arr[itor->level].count) {
        itor->pos = 0;
        --itor->level;
    }
    return f;
}

/* *************************** C++ version start ***************************** */

#ifdef __cplusplus


#endif /* __cplusplus */

typedef struct tagbddElementIterator {
    int sp;
    bddp* bddnode_stack;
    char* op_stack;
} bddElementIterator;

sbddextended_INLINE_FUNC
void bddElementIterator_destruct(bddElementIterator* itor)
{
    free(itor->op_stack);
    free(itor->bddnode_stack);
    free(itor);
}

sbddextended_INLINE_FUNC
int bddElementIterator_hasNext(const bddElementIterator* itor)
{
    return (itor->sp >= 0 ? 1 : 0);
}

sbddextended_INLINE_FUNC
void bddElementIterator_proceed(bddElementIterator* itor)
{
    char op;
    bddp node, child;

    while (itor->sp >= 0) {
        node = itor->bddnode_stack[itor->sp];
        op = itor->op_stack[itor->sp];
        if (node == bddempty || op == 2) {
            --(itor->sp);
            if (itor->sp >= 0) {
                ++itor->op_stack[itor->sp];
            }
        } else if (node == bddsingle) {
            break;
        } else {
            if (op == 0) {
                child = bddgetchild1z(node);
            } else {
                child = bddgetchild0z(node);
            }
            ++(itor->sp);
            itor->bddnode_stack[itor->sp] = child;
            itor->op_stack[itor->sp] = 0;
        }
    }
}

sbddextended_INLINE_FUNC
bddElementIterator* bddElementIterator_make(bddp f)
{
    int height;
    bddElementIterator* itor;

    itor = (bddElementIterator*)malloc(sizeof(bddElementIterator));
    if (itor == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    if (f == bddnull || f == bddempty) {
        itor->sp = -1;
        itor->bddnode_stack = NULL;
        itor->op_stack = NULL;
        return itor;
    }

    height = (int)bddgetlev(f) + 1;
    itor->bddnode_stack = (bddp*)malloc((size_t)height * sizeof(bddp));
    if (itor->bddnode_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    itor->op_stack = (char*)malloc((size_t)height * sizeof(char));
    if (itor->op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    itor->sp = 0;
    itor->bddnode_stack[itor->sp] = f;
    itor->op_stack[itor->sp] = 0;

    bddElementIterator_proceed(itor);

    return itor;
}

sbddextended_INLINE_FUNC
void bddElementIterator_getValue(bddElementIterator* itor, bddvar* arr)
{
    int i, c;

    c = 0;
    for (i = 0; i < itor->sp; ++i) {
        if (itor->op_stack[i] == 0) {
            arr[c] = bddgetvar(itor->bddnode_stack[i]);
            ++c;
        }
    }
    arr[c] = (bddvar)-1;
}

sbddextended_INLINE_FUNC
void bddElementIterator_next(bddElementIterator* itor, bddvar* arr)
{
    if (arr != NULL) {
        bddElementIterator_getValue(itor, arr);
    }
    --itor->sp;
    if (itor->sp >= 0) {
        ++itor->op_stack[itor->sp];
        bddElementIterator_proceed(itor);
    }
}

#ifdef __cplusplus

#if __cplusplus >= 201703L
class ElementIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::set<bddvar>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::set<bddvar>*;
    using reference = std::set<bddvar>&;
private:
#else
class ElementIterator
    : public std::iterator<std::input_iterator_tag, std::set<bddvar> > {
private:
#endif
    int sp_;
    std::vector<bddp> bddnode_stack_;
    std::vector<char> op_stack_;
    mutable std::set<bddvar> buffSet_;

    /* This function is written by copy-paste of void */
    /* bddElementIterator_proceed(bddElementIterator* itor). */
    void proceed()
    {
        char op;
        bddp node, child;

        while (sp_ >= 0) {
            node = bddnode_stack_[sp_];
            op = op_stack_[sp_];
            if (node == bddempty || op == 2) {
                --sp_;
                if (sp_ >= 0) {
                    ++op_stack_[sp_];
                }
            } else if (node == bddsingle) {
                break;
            } else {
                if (op == 0) {
                    child = bddgetchild1z(node);
                } else {
                    child = bddgetchild0z(node);
                }
                ++sp_;
                bddnode_stack_[sp_] = child;
                op_stack_[sp_] = 0;
            }
        }
    }

    void setToBuff()
    {
        buffSet_.clear();
        if (sp_ >= 0) {
            for (int i = 0; i < sp_; ++i) {
                if (op_stack_[i] == 0) {
                    buffSet_.insert(bddgetvar(bddnode_stack_[i]));
                }
            }
        }
    }

public:
    ElementIterator(bddp f, bool is_end)
    {
        if (is_end || f == bddnull || f == bddempty) {
            sp_ = -1;
        } else {
            sp_ = 0;
            int height = (int)bddgetlev(f) + 1;
            bddnode_stack_.resize(height);
            op_stack_.resize(height);

            bddnode_stack_[sp_] = f;
            op_stack_[sp_] = 0;

            proceed();

            setToBuff();
        }
    }

    std::set<bddvar> operator*() const
    {
        return buffSet_;
    }

    const std::set<bddvar>* operator->() const
    {
        return &buffSet_;
    }

    ElementIterator& operator++()
    {
        --sp_;
        if (sp_ >= 0) {
            ++op_stack_[sp_];
            proceed();
            setToBuff();
        }
        return *this;
    }

    bool operator==(const ElementIterator& it) const
    {
        if (sp_ >= 0) {
            if (sp_ != it.sp_) {
                return false;
            }
            for (int i = 0; i < sp_; ++i) {
                if (bddnode_stack_[i] != it.bddnode_stack_[i]) {
                    return false;
                }
                if (op_stack_[i] != it.op_stack_[i]) {
                    return false;
                }
            }
            return true;
        } else {
            return it.sp_ < 0;
        }
    }

    bool operator!=(const ElementIterator& it) const
    {
        return !(operator==(it));
    }
};

class ElementIteratorHolder {
private:
    bddp f_;
public:
    ElementIteratorHolder(bddp f) {
        f_ = f;
    }

    ElementIteratorHolder(const ZBDD& f) {
        f_ = f.GetID();
    }

    ElementIterator begin() const {
        return ElementIterator(f_, false);
    }

    ElementIterator end() const {
        return ElementIterator(f_, true);
    }
};

#endif

/* *************** import functions */

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements_inner(FILE* fp
#ifdef __cplusplus
                    , ReadCharObject& sbddextended_readChar
#endif
                                        )
{
    bddp p, q, r;
    bddvar* vararr;
    int vararr_pos, vararr_size;
    int c, prev_c, v;
    int mode, first;

    vararr_size = sbddextended_BUFSIZE;
    vararr = (bddvar*)malloc((size_t)vararr_size * sizeof(bddvar));
    if (vararr == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    vararr_pos = 0;

    p = bddfalse;
    first = 1;

    mode = 0; /* 0: skip ws, 1: reading nums */
    v = 0;
    c = 0;
    while (c != -1) {
        prev_c = c;
        c = sbddextended_readChar(fp);
        if (first == 1) {
            first = 0;
            if (c == 'T') {
                p = bddtrue;
                break;
            } else if (c == 'B' || c == 'E' || c == 'F') {
                break;
            }
        }
        if (c != -1) {
            if (!isdigit(c) && c != '\n' && !isspace(c)) {
                fprintf(stderr, "invalid char\n");
                exit(1);
            }
            if (mode == 0) {
                if (isdigit(c)) {
                    v = c - '0';
                    mode = 1;
                }
            } else if (mode == 1) {
                if (isdigit(c)) {
                    v = 10 * v + (c - '0');
                }
            }
        }
        if ((c == -1 || c == '\n' || isspace(c)) && mode == 1) {
            if (vararr_pos >= vararr_size) {
                vararr_size *= 2;
                vararr = (bddvar*)realloc(vararr, (size_t)vararr_size * sizeof(bddvar));
                if (vararr == NULL) {
                    fprintf(stderr, "out of memory\n");
                    exit(1);
                }
            }
            vararr[vararr_pos] = (bddvar)v;
            ++vararr_pos;
            mode = 0;
        }
        if ((c == -1 && prev_c != '\n') || c == '\n') {
            q = bddgetsingleset(vararr, vararr_pos);
            r = bddunion(p, q);
            bddfree(p);
            bddfree(q);
            p = r;
            vararr_pos = 0;
        }
    }
    free(vararr);
    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(FILE* fp)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructzbddfromelements_inner(fp, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(std::istream& ist)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructzbddfromelements_inner(NULL, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp)
{
    ReadLineObject glo;
    return bddconstructzbddfromelements_inner(fp, glo);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(FILE* /*fp*/, const char* /*large_sep*/,
                                const char* /*small_sep*/)
{
    std::cerr << "Arguments large_sep and small_sep are no longer supported.";
    exit(1);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(std::istream& /*ist*/, const char* /*large_sep*/,
                                const char* /*small_sep*/)
{
    std::cerr << "Arguments large_sep and small_sep are no longer supported.";
    exit(1);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* /*fp*/, const char* /*large_sep*/,
                                    const char* /*small_sep*/)
{
    std::cerr << "Arguments large_sep and small_sep are no longer supported.";
    exit(1);
}

#else

/* The following function is no longer supported.
sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp, const char* large_sep,
                                    const char* small_sep)
{
    return bddconstructzbddfromelements_inner(fp, large_sep, small_sep);
}*/

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp)
{
    return bddconstructzbddfromelements_inner(fp);
}

#endif


/* *************** export functions */


/* num_of_variables: 0 -> elements format, non 0 -> value list format */
sbddextended_INLINE_FUNC
void bddprintzbddelements_inner(FILE* fp, bddp f, const char* delim1,
                                const char* delim2, const char* var_name_map[],
                                int num_of_variables
#ifdef __cplusplus
                       , const WriteObject& sbddextended_write
#endif
                          )
{
    bddp* bddnode_stack;
    bddp g;
    char* op_stack;
    char* value_list = NULL;
    char op;
    int i, height, sp, is_first_delim1, is_first_delim2;
    bddvar v;
    bddp node, child;
    char buf[sbddextended_BUFSIZE];

    if (f == bddnull) {
        sbddextended_write("N", fp);
        return;
    } else if (f == bddempty) {
        sbddextended_write("E", fp);
        return;
    } else if (f == bddsingle) {
        if (num_of_variables != 0) {
            for (i = 1; i <= num_of_variables; ++i) {
                sbddextended_snprintf0(buf, sbddextended_BUFSIZE, "0");
                sbddextended_write(buf, fp);
                if (i < num_of_variables) {
                    sbddextended_snprintf1(buf, sbddextended_BUFSIZE, "%s", delim2);
                    sbddextended_write(buf, fp);
                }
            }
        }
        return;
    }

    is_first_delim1 = 1;
    if (bddisnegative(f)) {
        /* Output bddsingle first */
        if (num_of_variables != 0) {
            for (i = 1; i <= num_of_variables; ++i) {
                sbddextended_snprintf0(buf, sbddextended_BUFSIZE, "0");
                sbddextended_write(buf, fp);
                if (i < num_of_variables) {
                    sbddextended_snprintf1(buf, sbddextended_BUFSIZE, "%s", delim2);
                    sbddextended_write(buf, fp);
                }
            }
        }
        is_first_delim1 = 0;
        g = bddtakenot(f);
    } else {
        g = f;
    }

    height = (int)bddgetlev(g);
    bddnode_stack = (bddp*)malloc((size_t)(height + 1) * sizeof(bddp));
    if (bddnode_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    op_stack = (char*)malloc((size_t)(height + 1) * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }
    if (num_of_variables != 0) {
        value_list = (char*)malloc((size_t)(num_of_variables + 1) * sizeof(char));
        if (value_list == NULL) {
            fprintf(stderr, "out of memory\n");
            return;
        }
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
                sbddextended_snprintf1(buf, sbddextended_BUFSIZE, "%s", delim1);
                sbddextended_write(buf, fp);
            }
            is_first_delim2 = 1;
            if (num_of_variables != 0) {
                for (i = 1; i <= num_of_variables; ++i) {
                    value_list[i] = 0;
                }
                for (i = 0; i < sp; ++i) {
                    if (op_stack[i] == 0) {
                        v = bddgetvar(bddnode_stack[i]);
                        if (! (1 <= v && v <= (bddvar)num_of_variables)) {
                            fprintf(stderr, "variable number is "
                                "not in {1,...,num_of_variables} \n");
                            return;
                        }
                        value_list[v] = 1;
                    }
                }
                for (i = 1; i <= num_of_variables; ++i) {
                    sbddextended_snprintf1(buf, sbddextended_BUFSIZE, "%d", value_list[i]);
                    sbddextended_write(buf, fp);
                    if (i < num_of_variables) {
                        sbddextended_snprintf1(buf, sbddextended_BUFSIZE, "%s", delim2);
                        sbddextended_write(buf, fp);
                    }
                }
            } else {
                for (i = 0; i < sp; ++i) {
                    if (op_stack[i] == 0) {
                        if (is_first_delim2 != 0) {
                            is_first_delim2 = 0;
                        } else {
                            sbddextended_snprintf1(buf, sbddextended_BUFSIZE,
                                "%s", delim2);
                            sbddextended_write(buf, fp);
                        }
                        if (var_name_map != NULL) {
                            sbddextended_snprintf1(buf, sbddextended_BUFSIZE,
                                "%s",
                                var_name_map[bddgetvar(bddnode_stack[i])]);
                        } else {
                            sbddextended_snprintf1(buf, sbddextended_BUFSIZE,
                                "%d",
                                bddgetvar(bddnode_stack[i]));
                        }
                        sbddextended_write(buf, fp);
                    }
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
    if (num_of_variables != 0) {
        free(value_list);
    }
    free(op_stack);
    free(bddnode_stack);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd,
    const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(),
        delim2.c_str(), NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1,
    const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(false, false, NULL);
    const char** arr = sbddextended_strVectorToArray(var_name_map);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(),
        delim2.c_str(), arr, 0, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd,
    const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(),
    delim2.c_str(), NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd,
    const std::string& delim1, const std::string& delim2,
    const std::vector<std::string>& var_name_map)
{
    WriteObject wo(true, false, &ost);
    const char** arr = sbddextended_strVectorToArray(var_name_map);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(),
        delim2.c_str(), arr, 0, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd)
{
    fprintf(fp, "{");
    printZBDDElements(fp, zbdd, "}, {", ", ");
    fprintf(fp, "}");
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd)
{
    ost << "{";
    printZBDDElements(ost, zbdd, "}, {", ", ");
    ost << "}";
}

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1,
    const char* delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1,
    const char* delim2, const char* var_name_map[])
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, 0, wo);
}

#else

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1,
    const char* delim2)
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, 0);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1,
    const char* delim2, const char* var_name_map[])
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, 0);
}

#endif


#ifdef __cplusplus

sbddextended_INLINE_FUNC
void printZBDDElementsAsValueList(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, int num_of_variables)
{
    if (num_of_variables < 0) {
        fprintf(stderr, "num_of_variables must be at least 1\n");
        return;
    }

    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(),
                               delim2.c_str(), NULL, num_of_variables, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElementsAsValueList(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, int num_of_variables)
{
    if (num_of_variables < 0) {
        std::cerr << "num_of_variables must be at least 1" << std::endl;
        return;
    }

    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, num_of_variables, wo);
}

sbddextended_INLINE_FUNC
std::string ZStr(const ZBDD& zbdd)
{
    if (zbdd == ZBDD(-1)) { /* null */
        return std::string("N");
    } if (zbdd == ZBDD(0)) { /* 0-terminal */
        return std::string("E");
    } else {
        std::ostringstream ost;
        printZBDDElements(ost, zbdd);
        return ost.str();
    }
}

sbddextended_INLINE_FUNC
std::string zstr(const ZBDD& zbdd)
{
    return ZStr(zbdd);
}

#endif

/* *************** import functions */

sbddextended_INLINE_FUNC
bddp bddimportbddasbinary_inner(FILE* fp, int root_level, int is_zbdd
#ifdef __cplusplus
                                , ReadCharObject& sbddextended_readUint8
                                , ReadCharObject& sbddextended_readUint16
                                , ReadCharObject& sbddextended_readUint32
                                , ReadCharObject& sbddextended_readUint64
#endif
                                )
{
    ullint i, level, max_level, root_id, number_of_nodes;
    ullint node_count, node_sum;
    unsigned int number_of_terminals;
    bddvar var;
    bddp f, f0, f1;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec;
    unsigned char use_negative_arcs;
    unsigned char v8;
    unsigned short v16;
    ullint v64;

    /* read head 'B' 'D' 'D' */
    for (i = 0; i < 3; ++i) {
        sbddextended_readUint8(&v8, fp);
        if ((i == 0 && v8 != 'B') || (i >= 1 && v8 != 'D')) {
            fprintf(stderr, "This binary is not in the BDD binary format.\n");
            return bddnull;
        }
    }
    sbddextended_readUint8(&v8, fp); /* version */
    if (v8 != 1) {
        fprintf(stderr, "This function supports only version 1.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); /* type */
    if (is_zbdd < 0 && v8 == 1) {
        fprintf(stderr, "Need to specify BDD or ZBDD.\n");
        return bddnull;
    } else if (is_zbdd > 0 && v8 == 2) {
        fprintf(stderr, "The binary indicates that it is BDD, but we interpret it as a ZBDD.\n");
    } else if (is_zbdd == 0 && v8 == 3) {
        fprintf(stderr, "The binary indicates that it is ZDDD, but we interpret it as a BDD.\n");
    }

    sbddextended_readUint16(&v16, fp); /* number_of_arcs */
    if (v16 != 2) {
        fprintf(stderr, "Currently, this function supports only 2 branches.\n");
        return bddnull;
    }

    sbddextended_readUint32(&number_of_terminals, fp); /* number_of_terminals */
    if (number_of_terminals != 2) {
        fprintf(stderr, "Currently, this function supports only 2 terminals.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); /* number_of_bits_for_level */
    if (v8 != 16) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_level == 16.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); /* number_of_bits_for_id */
    if (v8 != 64) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_id == 64.\n");
        return bddnull;
    }

    sbddextended_readUint8(&use_negative_arcs, fp); /* use_negative_arcs */
    sbddextended_readUint64(&max_level, fp); /* max_level */

    if (root_level < 0) {
        root_level = (int)max_level;
    } else if (root_level < (int)max_level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "at least the height of the ZBDD.\n");
        return bddnull;
    }

    sbddextended_readUint64(&v64, fp); /* number_of_roots */
    if (v64 != 1) {
        fprintf(stderr, "Currently, this function supports only 1 root.\n");
        return bddnull;
    }
    /* reserved */
    for (i = 0; i < 8; ++i) {
        sbddextended_readUint64(&v64, fp);
    }

    if (max_level == 0) { /* case of a constant function (0/1-terminal) */
        sbddextended_readUint64(&v64, fp);
        if (v64 == 0) {
            return bddempty;
        } else if (v64 == 1) {
            return bddsingle;
        } else {
            fprintf(stderr, "Currently, this function supports only 0/1-terminal.\n");
            return bddnull;
        }
    }

    sbddextended_MyVector_initialize(&level_vec);

    /* level 0, unused (dummy) */
    sbddextended_MyVector_add(&level_vec, 0ll);

    number_of_nodes = number_of_terminals;
    for (level = 1; level <= max_level; ++level) {
        sbddextended_readUint64(&v64, fp);
        sbddextended_MyVector_add(&level_vec, (llint)v64);
        number_of_nodes += v64;
    }
    sbddextended_readUint64(&root_id, fp);

    assert(number_of_nodes >= number_of_terminals);
    assert(number_of_nodes >= 2);
    bddnode_buf = (bddp*)malloc((size_t)number_of_nodes * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = bddempty;
    bddnode_buf[1] = bddsingle;

    while (bddvarused() < (bddvar)root_level) {
        bddnewvar();
    }

    for (node_count = number_of_terminals;
            node_count < number_of_nodes; ++node_count) {
        /* read 0-child */
        if (!sbddextended_readUint64(&v64, fp)) {
            break;
        }

        if (v64 <= 1) {
            f0 = bddgetterminal((int)v64, is_zbdd);
        } else {
            if (use_negative_arcs != 0) {
                if(v64 % 2 == 1) {
                    fprintf(stderr, "0-child must not be negative.\n");
                    return bddnull;
                }
                f0 = bddnode_buf[v64 >> 1];
            } else {
                f0 = bddnode_buf[v64];
            }
        }

        /* read 1-child */
        if (!sbddextended_readUint64(&v64, fp)) {
            fprintf(stderr, "illegal format\n");
            return bddnull;
        }
        if (v64 <= 1) {
            f1 = bddgetterminal((int)v64, is_zbdd);
        } else {
            if (use_negative_arcs != 0) {
                f1 = bddnode_buf[v64 >> 1];
                if (v64 % 2 == 1) {
                    f1 = bddtakenot(f1);
                }
            } else {
                f1 = bddnode_buf[v64];
            }
        }

        /* obtain node's level */
        node_sum = number_of_terminals;
        for (level = 1; level <= max_level; ++level) {
            /* add the number of nodes at the level */
            node_sum += (ullint)sbddextended_MyVector_get(&level_vec, (llint)level);
            if (node_sum > node_count) {
                break;
            }
        }
        assert(level <= max_level);
        var = bddvaroflev((bddvar)((int)level + root_level - (int)max_level));
        assert(node_count < number_of_nodes);
        if (is_zbdd != 0) {
            bddnode_buf[node_count] = bddmakenodez(var, f0, f1);
        } else {
            bddnode_buf[node_count] = bddmakenodeb(var, f0, f1);
        }
    }
    if (use_negative_arcs != 0) {
        if (root_id % 2 == 1) { /* negative arc */
            f = bddtakenot(bddcopy(bddnode_buf[root_id >> 1]));
        } else {
            f = bddcopy(bddnode_buf[root_id >> 1]);
        }
    } else {
        f = bddcopy(bddnode_buf[root_id]);
    }
    /* FIX ME: need to free of bddnode_buf[*] */
    for (node_count = number_of_terminals;
            node_count < number_of_nodes; ++node_count) {
        bddfree(bddnode_buf[node_count]);
    }
    return f;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
BDD importBDDAsBinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasbinary_inner(fp,
                                    root_level, 0,
                                    glo, glo, glo, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD importBDDAsBinary(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasbinary_inner(NULL,
                                    root_level, 0,
                                    glo, glo, glo, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsBinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasbinary_inner(fp,
                                    root_level, 1,
                                    glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsBinary(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasbinary_inner(NULL,
                                    root_level, 1,
                                    glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddimportbddasbinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasbinary_inner(fp,
                                        root_level, 0,
                                        glo, glo, glo, glo);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasbinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasbinary_inner(fp,
                                        root_level, 1,
                                        glo, glo, glo, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddimportbddasbinary(FILE* fp, int root_level)
{
    return bddimportbddasbinary_inner(fp, root_level, 0);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasbinary(FILE* fp, int root_level)
{
    return bddimportbddasbinary_inner(fp, root_level, 1);
}

#endif


/* *************** export functions */


sbddextended_INLINE_FUNC
void bddexportbddasbinary_inner(FILE* fp, bddp f,
                                bddNodeIndex* node_index,
                                int is_zbdd,
                                int use_negative_arcs
#ifdef __cplusplus
                                , const WriteObject& sbddextended_writeUint8
                                , const WriteObject& sbddextended_writeUint16
                                , const WriteObject& sbddextended_writeUint32
                                , const WriteObject& sbddextended_writeUint64
#endif
                                )
{
    /* Since only BDD/ZDD is treated in the current version, */
    /* the number of terminals is fixed to be 2. */
    const unsigned int number_of_terminals = 2;
    ullint i, j;
    int k, is_making_index = 0;
    ullint max_level;
    ullint number_of_nodes = 0;
    ullint root_id;
    llint id;
    ullint* id_prefix;
    bddp node, child, rchild;

    if (node_index != NULL) {
        if (node_index->is_raw != 0 && use_negative_arcs == 0) {
            fprintf(stderr, "The node index must not be constructed in the raw mode "
                            "when not using negative arcs.\n");
            return;
        } else if (node_index->is_raw == 0 && use_negative_arcs != 0) {
            fprintf(stderr, "The node index must be constructed in the raw mode "
                            "when using negative arcs.\n");
            return;
        }
    }

    max_level = (ullint)bddgetlev(f);

    /* start header */

    sbddextended_writeUint8((unsigned char)'B', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);

    sbddextended_writeUint8((unsigned char)1u, fp); /* version */
    /* DD type */
    if (is_zbdd < 0) { /* can be interpreted as BDD/ZBDD */
        sbddextended_writeUint8((unsigned char)1u, fp);
    } else if (is_zbdd == 0) { /* BDD */
        sbddextended_writeUint8((unsigned char)2u, fp);
    } else { /* ZBDD */
        sbddextended_writeUint8((unsigned char)3u, fp);
    }
    /* number_of_arcs */
    sbddextended_writeUint16((unsigned short)2u, fp);
    /* number_of_terminals */
    sbddextended_writeUint32(number_of_terminals, fp);
    /* number_of_bits_for_level */
    sbddextended_writeUint8((unsigned char)16u, fp);
    /* number_of_bits_for_id */
    sbddextended_writeUint8((unsigned char)64u, fp);
    /* use_negative_arcs */
    if (use_negative_arcs != 0) {
        sbddextended_writeUint8((unsigned char)1u, fp);
    } else {
        sbddextended_writeUint8((unsigned char)0u, fp);
    }
    /* max_level */
    sbddextended_writeUint64(max_level, fp);
    /* number_of_roots */
    sbddextended_writeUint64((ullint)1u, fp);

    /* reserved */
    for (i = 0; i < 8; ++i) {
        sbddextended_writeUint64((ullint)0u, fp);
    }

    /* end header */

    if (is_zbdd < 0) {
        is_zbdd = (!bddisbdd(f) ? 1 : 0);
    }

    if (max_level == 0) { /* case of a constant function (0/1-terminal) */
        if (f == bddempty) {
            sbddextended_writeUint64((ullint)0ull, fp);
        } else if (f == bddsingle) {
            sbddextended_writeUint64((ullint)1ull, fp);
        } else {
            assert(0);
        }
        return;
    }

    if (node_index == NULL) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            if (use_negative_arcs != 0) {
                node_index = bddNodeIndex_makeRawIndexZWithoutCount(f);
            } else {
                node_index = bddNodeIndex_makeIndexZWithoutCount(f);
            }
        } else {
            if (use_negative_arcs != 0) {
                node_index = bddNodeIndex_makeRawIndexBWithoutCount(f);
            } else {
                node_index = bddNodeIndex_makeIndexBWithoutCount(f);
            }
        }
    }

    assert((ullint)node_index->height == max_level);

    /* write the number of nodes in level i and compute the number of nodes */
    for (i = 1; i <= max_level; ++i) {
        sbddextended_writeUint64((ullint)node_index->level_vec_arr[i].count, fp);
        number_of_nodes += (ullint)node_index->level_vec_arr[i].count;
    }

    id_prefix = (ullint*)malloc((size_t)(max_level + 1) * sizeof(ullint));
    if (id_prefix == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    id_prefix[1] = number_of_terminals;
    for (i = 1; i < max_level; ++i) {
        id_prefix[i + 1] = id_prefix[i] + (ullint)node_index->level_vec_arr[i].count;
    }

    /* write the number of the root id */
    /* (In the current version, assume that the number of roots is 2.) */
    root_id = (ullint)number_of_terminals + number_of_nodes - 1;
    assert(root_id == id_prefix[max_level]);

    if (use_negative_arcs != 0) {
        root_id *= 2;
        if (bddisnegative(f)) {
            ++root_id;
        }
    }

    sbddextended_writeUint64(root_id, fp);

    for (i = 1; i <= max_level; ++i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildg(node, k, node_index->is_zbdd, node_index->is_raw);
                if (child == bddempty) {
                    sbddextended_writeUint64(0llu, fp);
                } else if (child == bddsingle) {
                    sbddextended_writeUint64(1llu, fp);
                } else {
                    rchild = (use_negative_arcs != 0 ? bdderasenot(child) : child);
                    if (sbddextended_MyDict_find(&node_index->node_dict_arr[bddgetlev(child)],
                                                    (llint)rchild, &id) == 0) {
                        fprintf(stderr, "node not found\n");
                        exit(1);
                    }
                    id += (llint)id_prefix[bddgetlev(child)];
                    if (use_negative_arcs != 0) {
                        id *= 2;
                        if (bddisnegative(child)) {
                            id += 1;
                        }
                    }
                    sbddextended_writeUint64((ullint)id, fp);
                }
            }
        }
    }

    if (is_making_index) {
        bddNodeIndex_destruct(node_index);
        free(node_index);
    }
    free(id_prefix);
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsBinary(FILE* fp, const BDD& bdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, bdd.GetID(), bnode_index,
                                0, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsBinary(FILE* fp, const BDD& bdd, bool use_negative_arcs = true)
{
    exportBDDAsBinary<int>(fp, bdd, use_negative_arcs, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsBinary(std::ostream& ost, const BDD& bdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasbinary_inner(NULL, bdd.GetID(), bnode_index,
                                0, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsBinary(std::ostream& ost, const BDD& bdd, bool use_negative_arcs = true)
{
    exportBDDAsBinary<int>(ost, bdd, use_negative_arcs, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, zbdd.GetID(), bnode_index,
                                1, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsBinary(FILE* fp, const ZBDD& zbdd, bool use_negative_arcs = true)
{
    exportZBDDAsBinary<int>(fp, zbdd, use_negative_arcs, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsBinary(std::ostream& ost, const ZBDD& zbdd, bool use_negative_arcs, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasbinary_inner(NULL, zbdd.GetID(), bnode_index,
                                1, (use_negative_arcs ? 1 : 0),
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsBinary(std::ostream& ost, const ZBDD& zbdd, bool use_negative_arcs = true)
{
    exportZBDDAsBinary<int>(ost, zbdd, use_negative_arcs, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, f, node_index,
                                0, use_negative_arcs,
                                wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasbinary_inner(fp, f, node_index,
                                1, use_negative_arcs,
                                wo, wo, wo, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    bddexportbddasbinary_inner(fp, f, node_index, 0, use_negative_arcs);
}

sbddextended_INLINE_FUNC
void bddexportzbddasbinary(FILE* fp, bddp f, int use_negative_arcs, bddNodeIndex* node_index)
{
    bddexportbddasbinary_inner(fp, f, node_index, 1, use_negative_arcs);
}

#endif

/* *************** import functions */

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion_inner(FILE* fp, int root_level, int is_zdd
#ifdef __cplusplus
                                            , ReadLineObject& sbddextended_readLine
#endif
                                            )
{
    int c, level, max_level = 0;
    llint i, id, lo, hi, value, line_count = 0;
    llint max_node_id = 0, root_node_id = 0;
    bddvar var;
    char buf[sbddextended_BUFSIZE];
    char buf1[sbddextended_BUFSIZE];
    char buf2[sbddextended_BUFSIZE];
    char buf3[sbddextended_BUFSIZE];
    char buf4[sbddextended_BUFSIZE];
    bddp p, p0, p1, pf, pfn;
    sbddextended_MyVector node_vec, level_vec, lo_vec, hi_vec;
    sbddextended_MyDict node_dict;

    sbddextended_readLine(buf, fp); /* read first line */
    if (buf[0] == '.') { /* end of file */
        return bddnull;
    } else if (buf[0] == 'B' && (buf[1] == '\0' || isspace((int)buf[1]))) {
        return (is_zdd == 0 ? bddfalse : bddempty);
    } else if (buf[0] == 'T' && (buf[1] == '\0' || isspace((int)buf[1]))) {
        return (is_zdd == 0 ? bddtrue : bddsingle);
    }

    sbddextended_MyVector_initialize(&node_vec);
    sbddextended_MyVector_initialize(&level_vec);
    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    do {
        ++line_count;
        if (buf[0] == '.') { /* end of file */
            break;
        }
        c = sscanf(buf, "%s %s %s %s", buf1, buf2, buf3, buf4);
        if (c < 4) {
            fprintf(stderr, "Format error in line %lld\n", line_count);
            return bddnull;
        }
        if (buf3[0] == 'B') {
            lo = 0;
        } else if (buf3[0] == 'T') {
            lo = 1;
        } else {
            lo = strtoll(buf3, NULL, 10) + 2;
        }
        if (buf4[0] == 'B') {
            hi = 0;
        } else if (buf4[0] == 'T') {
            hi = 1;
        } else {
            hi = strtoll(buf4, NULL, 10) + 2;
        }
        id = strtoll(buf1, NULL, 10) + 2;
        level = (int)strtol(buf2, NULL, 10);
        sbddextended_MyVector_add(&node_vec, id);
        sbddextended_MyVector_add(&level_vec, (llint)level);
        sbddextended_MyVector_add(&lo_vec, lo);
        sbddextended_MyVector_add(&hi_vec, hi);
        if (max_node_id < id) {
            max_node_id = id;
        }
        if (max_level < level) {
            max_level = level;
        }
    } while (sbddextended_readLine(buf, fp));

    if (root_level < 0) {
        root_level = max_level;
    } else if (root_level < max_level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        return bddnull;
    }

    while ((int)bddvarused() < max_level) {
        bddnewvar();
    }

    sbddextended_MyDict_initialize(&node_dict);
    sbddextended_MyDict_add(&node_dict, 0ll, (is_zdd == 0 ? bddfalse : bddempty));
    sbddextended_MyDict_add(&node_dict, 1ll, (is_zdd == 0 ? bddtrue : bddsingle));

    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        level = (int)sbddextended_MyVector_get(&level_vec, i);
        lo = sbddextended_MyVector_get(&lo_vec, i);
        hi = sbddextended_MyVector_get(&hi_vec, i);
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { /* BDD */
            pf = bddprime(var);
            pfn = bddnot(pf);
            sbddextended_MyDict_find(&node_dict, lo, &value);
            p0 = bddand((bddp)value, pfn);
            sbddextended_MyDict_find(&node_dict, hi, &value);
            p1 = bddand((bddp)value, pf);
            sbddextended_MyDict_add(&node_dict, id, (llint)bddor(p0, p1));
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { /* ZDD */
            sbddextended_MyDict_find(&node_dict, lo, &value);
            p0 = (bddp)value;
            sbddextended_MyDict_find(&node_dict, hi, &value);
            p1 = bddchange((bddp)value, var);
            sbddextended_MyDict_add(&node_dict, id, (llint)bddunion(p0, p1));
            bddfree(p1);
        }
        root_node_id = id; /* The root node is the last node. */
    }
    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        if (id != root_node_id) {
            sbddextended_MyDict_find(&node_dict, id, &value);
            bddfree((bddp)value);
        }
    }

    sbddextended_MyDict_find(&node_dict, root_node_id, &value);
    p = (bddp)value;

    sbddextended_MyDict_deinitialize(&node_dict);
    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);
    sbddextended_MyVector_deinitialize(&node_vec);

    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
BDD importBDDAsGraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasgraphillion_inner(fp, root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD importBDDAsGraphillion(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasgraphillion_inner(NULL, root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsGraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasgraphillion_inner(fp, root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsGraphillion(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasgraphillion_inner(NULL, root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasgraphillion_inner(fp, root_level, 0, glo);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasgraphillion(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasgraphillion_inner(fp, root_level, 1, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddimportbddasgraphillion(FILE* fp, int root_level)
{
    return bddimportbddasgraphillion_inner(fp, root_level, 0);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasgraphillion(FILE* fp, int root_level)
{
    return bddimportbddasgraphillion_inner(fp, root_level, 1);
}

#endif

/* *************** export functions */


sbddextended_INLINE_FUNC
void bddexportbddasgraphillion_inner(FILE* fp, bddp f,
                                        bddNodeIndex* node_index, int is_zbdd,
                                        int root_level
#ifdef __cplusplus
                        , const WriteObject& sbddextended_writeLine
#endif
                                    )
{
    int i, k, n;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    char ss[sbddextended_BUFSIZE];

    if (f == bddnull) {
        return;
    }

    if (is_zbdd < 0 && !(f == bddempty || f == bddsingle)) {
        if (bddiszbdd(f) != 0) {
            is_zbdd = 1;
        } else {
            is_zbdd = 0;
        }
    }

    if (node_index != NULL) {
        if (node_index->is_raw != 0) {
            fprintf(stderr, "The node index must be constructed in the raw mode ");
            return;
        }
    }

    if (node_index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            node_index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            node_index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    if (f == bddempty) {
        sbddextended_writeLine("B", fp);
        sbddextended_writeLine(".", fp);
        return;
    } else if (f == bddsingle) {
        sbddextended_writeLine("T", fp);
        sbddextended_writeLine(".", fp);
        return;
    }
    if (root_level < 0) {
        root_level = (int)bddlevofvar(bddtop(f));
    }

    for (i = 1; i <= node_index->height; ++i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], (llint)j);
            n = sbddextended_snprintf2(ss, sbddextended_BUFSIZE, "%lld %d", (llint)node, (root_level - i + 1));
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildg(node, k, is_zbdd, 0);
                if (!bddisterminal(child)) {
                    n += sbddextended_snprintf1(ss + n, sbddextended_BUFSIZE, " %lld", (llint)child);
                } else if (child == bddempty) {
                    n += sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE, " B");
                } else if (child == bddsingle) {
                    n += sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE, " T");
                }
            }
            sbddextended_writeLine(ss, fp);
        }
    }

    sbddextended_writeLine(".", fp);

    if (is_making_index) {
        bddNodeIndex_destruct(node_index);
        free(node_index);
    }
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(FILE* fp, const BDD& bdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphillion_inner(fp, bdd.GetID(), bnode_index, 0, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(FILE* fp, const BDD& bdd, int root_level = -1)
{
    exportBDDAsGraphillion<int>(fp, bdd, root_level, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(std::ostream& ost, const BDD& bdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphillion_inner(NULL, bdd.GetID(), bnode_index, 0, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphillion(std::ostream& ost, const BDD& bdd, int root_level = -1)
{
    exportBDDAsGraphillion<int>(ost, bdd, root_level, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(FILE* fp, const ZBDD& zbdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphillion_inner(fp, zbdd.GetID(), bnode_index, 1, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(FILE* fp, const ZBDD& zbdd, int root_level = -1)
{
    exportZBDDAsGraphillion<int>(fp, zbdd, root_level, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(std::ostream& ost, const ZBDD& zbdd, int root_level, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphillion_inner(NULL, zbdd.GetID(), bnode_index, 1, root_level, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphillion(std::ostream& ost, const ZBDD& zbdd, int root_level = -1)
{
    exportZBDDAsGraphillion<int>(ost, zbdd, root_level, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    WriteObject wo(false, true, NULL);
    /* can be used for BDD/ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, -1, root_level, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    WriteObject wo(false, true, NULL);
    /* only for ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, 1, root_level, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    /* can be used for BDD/ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, -1, root_level);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphillion(FILE* fp, bddp f,
                                bddNodeIndex* node_index, int root_level)
{
    /* only for ZBDD */
    bddexportbddasgraphillion_inner(fp, f, node_index, 1, root_level);
}

#endif

/* *************** import functions */

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth_inner(FILE* fp, int is_hex, int root_level,
                                int is_zbdd
#ifdef __cplusplus
                                , ReadLineObject& sbddextended_readLine
#endif
                                )
{
    int c, level, level_count = 1;
    llint i, id, lo, hi, line_count = 0;
    ullint idu, lou, hiu;
    bddvar var;
    char buf[sbddextended_BUFSIZE];
    bddp p, p0, p1, pf, pfn;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec, lo_vec, hi_vec;

    /* To avoid compiler warning, we initialize it here. */
    sbddextended_MyVector_initialize(&level_vec);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if ((buf[0] == '0' && buf[1] == '\n') ||
                (buf[0] == '0' && buf[1] == '\0')) {
            sbddextended_MyVector_deinitialize(&level_vec);
            return bddgetterminal(0, is_zbdd);
        } else if ((buf[0] == '1' && buf[1] == '\n') ||
                (buf[0] == '1' && buf[1] == '\0')) {
            sbddextended_MyVector_deinitialize(&level_vec);
            return bddgetterminal(1, is_zbdd);
        } else if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                return bddnull;
            }
            assert(level == level_count);
            ++level_count;
            sbddextended_MyVector_add(&level_vec, (llint)2);
            break;
        }
    }

    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    sbddextended_MyVector_add(&lo_vec, 0ll);
    sbddextended_MyVector_add(&lo_vec, 1ll);
    sbddextended_MyVector_add(&hi_vec, 0ll);
    sbddextended_MyVector_add(&hi_vec, 1ll);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '#') {
            c = sscanf(buf, "#%d", &level);
            if (c < 1) {
                fprintf(stderr, "Format error in line %lld\n", line_count);
                return bddnull;
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
                return bddnull;
            }
            if (id != (llint)lo_vec.count) {
                fprintf(stderr, "Format error in line %lld\n",
                        line_count);
                return bddnull;
            }
            sbddextended_MyVector_add(&lo_vec, lo);
            sbddextended_MyVector_add(&hi_vec, hi);
        }
    }
    sbddextended_MyVector_add(&level_vec, (llint)lo_vec.count);
    assert(level_count == (int)level_vec.count);

    if (root_level < 0) {
        root_level = level_count - 1;
    } else if (root_level < level_count - 1) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        return bddnull;
    }

    while (bddvarused() < (bddvar)root_level) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc(lo_vec.count * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = bddgetterminal(0, is_zbdd);
    bddnode_buf[1] = bddgetterminal(1, is_zbdd);

    for (i = (llint)lo_vec.count - 1; i >= sbddextended_BDDNODE_START; --i) {
        for (level = 1; level < (llint)level_vec.count; ++level) {
            if (sbddextended_MyVector_get(&level_vec, (llint)level - 1) <= i &&
                i < sbddextended_MyVector_get(&level_vec, (llint)level)) {
                break;
            }
        }
        assert(level < (llint)level_vec.count);
        assert((1 <= root_level - level + 1) && ((root_level - level + 1) <= (int)bddvarused()));
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zbdd == 0) { /* BDD */
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand(bddnode_buf[sbddextended_MyVector_get(&lo_vec, i)], pfn);
            p1 = bddand(bddnode_buf[sbddextended_MyVector_get(&hi_vec, i)], pf);
            bddnode_buf[i] = bddor(p0, p1);
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { /* ZDD */
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
BDD importBDDAsKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasknuth_inner(fp, (is_hex ? 1 : 0),
                                            root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD importBDDAsKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasknuth_inner(NULL, (is_hex ? 1 : 0),
                                            root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddimportbddasknuth_inner(fp, (is_hex ? 1 : 0),
                                            root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD importZBDDAsKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddimportbddasknuth_inner(NULL, (is_hex ? 1 : 0),
                                            root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 0, glo);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 1, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddimportbddasknuth(FILE* fp, int is_hex, int root_level)
{
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 0);
}

sbddextended_INLINE_FUNC
bddp bddimportzbddasknuth(FILE* fp, int is_hex, int root_level)
{
    return bddimportbddasknuth_inner(fp, is_hex, root_level, 1);
}

#endif


/* *************** export functions */


sbddextended_INLINE_FUNC
void bddexportbddasknuth_inner(FILE* fp, bddp f, int is_hex,
                                bddNodeIndex* node_index,
                                int is_zbdd
#ifdef __cplusplus
                                    , const WriteObject& sbddextended_writeLine
#endif
                                    )
{
    int clevel, i, is_making_index = 0;
    bddp node, n0, n1;
    llint id0, id1, k;
    char ss[sbddextended_BUFSIZE];

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

    if (is_zbdd < 0 && !(f == bddempty || f == bddsingle)) {
        if (bddiszbdd(f) != 0) {
            is_zbdd = 1;
        } else {
            is_zbdd = 0;
        }
    }

    if (node_index != NULL) {
        if (node_index->is_raw != 0) {
            fprintf(stderr, "The node index must be constructed in the raw mode ");
            return;
        }
    }

    if (node_index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            node_index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            node_index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    for (i = node_index->height; i >= 1; --i) {
        sbddextended_snprintf1(ss, sbddextended_BUFSIZE, "#%d", node_index->height - i + 1);
        sbddextended_writeLine(ss, fp);
        for (k = 0; k < (llint)node_index->level_vec_arr[i].count; ++k) {
            node = (bddp)sbddextended_MyVector_get(&node_index->level_vec_arr[i], k);
            n0 = bddgetchild0g(node, is_zbdd, 0);
            if (n0 == bddempty) {
                id0 = 0;
            } else if (n0 == bddsingle) {
                id0 = 1;
            } else {
                clevel = (int)bddgetlev(n0);
                if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                (llint)n0, &id0) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                id0 += node_index->offset_arr[clevel];
            }
            n1 = bddgetchild1g(node, is_zbdd, 0);
            if (n1 == bddempty) {
                id1 = 0;
            } else if (n1 == bddsingle) {
                id1 = 1;
            } else {
                clevel = (int)bddgetlev(n1);
                if (sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                (llint)n1, &id1) == 0) {
                    fprintf(stderr, "node not found!\n");
                    exit(1);
                }
                id1 += node_index->offset_arr[clevel];
            }
            if (is_hex) {
                sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                    "%llx:%llx,%llx", node_index->offset_arr[i] + k,
                    id0, id1);
            } else {
                sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                    "%lld:%lld,%lld", node_index->offset_arr[i] + k,
                    id0, id1);
            }
            sbddextended_writeLine(ss, fp);
        }
    }
    if (is_making_index) {
        bddNodeIndex_destruct(node_index);
        free(node_index);
    }
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsKnuth(FILE* fp, const BDD& bdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, bdd.GetID(), (is_hex ? 1 : 0), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsKnuth(FILE* fp, const BDD& bdd, bool is_hex = false)
{
    exportBDDAsKnuth<int>(fp, bdd, is_hex, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsKnuth(std::ostream& ost, const BDD& bdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasknuth_inner(NULL, bdd.GetID(), (is_hex ? 1 : 0), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsKnuth(std::ostream& ost, const BDD& bdd, bool is_hex = false)
{
    exportBDDAsKnuth<int>(ost, bdd, is_hex, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex = false)
{
    exportZBDDAsKnuth<int>(fp, zbdd, is_hex, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex, DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasknuth_inner(NULL, zbdd.GetID(), (is_hex ? 1 : 0), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex = false)
{
    exportZBDDAsKnuth<int>(ost, zbdd, is_hex, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, f, (is_hex ? 1 : 0), node_index, 0, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    bddexportbddasknuth_inner(fp, f, (is_hex ? 1 : 0), node_index, 1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    bddexportbddasknuth_inner(fp, f, is_hex, node_index, 0);
}

sbddextended_INLINE_FUNC
void bddexportzbddasknuth(FILE* fp, bddp f, int is_hex, bddNodeIndex* node_index)
{
    bddexportbddasknuth_inner(fp, f, is_hex, node_index, 1);
}

#endif


sbddextended_INLINE_FUNC
void bddexportbddasgraphviz_inner(FILE* fp, bddp f,
                                    bddNodeIndex* node_index, int is_zbdd
#ifdef __cplusplus
                        , const WriteObject& sbddextended_writeLine
#endif
                                    )
{
    int i, k, c, clevel, n;
    llint cvalue;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    char ss[sbddextended_BUFSIZE];

    if (f == bddnull) {
        return;
    }

    if (is_zbdd < 0 && !(f == bddempty || f == bddsingle)) {
        if (bddiszbdd(f) != 0) {
            is_zbdd = 1;
        } else {
            is_zbdd = 0;
        }
    }

    if (node_index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            node_index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            node_index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    sbddextended_writeLine("digraph {", fp);
    /* print terminals */
    if (f != bddsingle) {
        sbddextended_writeLine("\tt0 [label = \"0\", shape = box, "
            "style = filled, color = \"#81B65D\", "
            "fillcolor = \"#F6FAF4\", penwidth = 2.5, "
            "width = 0.4, height = 0.6, fontsize = 24];", fp);
    }
    if (f != bddempty) {
        sbddextended_writeLine("\tt1 [label = \"1\", shape = box, "
            "style = filled, color = \"#81B65D\", "
            "fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, "
            "height = 0.6, fontsize = 24];", fp);
    }
    if (f == bddempty || f == bddsingle) {
        sbddextended_writeLine("}", fp);
        return;
    }

    /* print vars and levels */
    sbddextended_snprintf1(ss, sbddextended_BUFSIZE, 
        "\tr%d [shape = plaintext, label = \"var level\"]",
        node_index->height + 1);
    sbddextended_writeLine(ss, fp);
    sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
        "\tr%d [shape = plaintext, label = \"%4d%7d\"]",
        node_index->height, bddvaroflev((bddvar)node_index->height),
        node_index->height);
    sbddextended_writeLine(ss, fp);
    sbddextended_snprintf2(ss, sbddextended_BUFSIZE,
        "\tr%d -> r%d [style = invis];", node_index->height + 1,
        node_index->height);
    sbddextended_writeLine(ss, fp);
    for (i = node_index->height; i >= 1; --i) {
        if (i > 1) {
            sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                "\tr%d [shape = plaintext, label = \"%4d%7d\"];",
                i - 1, bddvaroflev((bddvar)(i - 1)), i - 1);
            sbddextended_writeLine(ss, fp);
        } else {
            sbddextended_writeLine("\tr0 [style = invis];", fp);
        }
        sbddextended_snprintf2(ss, sbddextended_BUFSIZE,
            "\tr%d -> r%d [style = invis];", i, i - 1);
        sbddextended_writeLine(ss, fp);
    }

    for (i = node_index->height; i >= 1; --i) {
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(
                &node_index->level_vec_arr[i], (llint)j);
            sbddextended_snprintf2(ss, sbddextended_BUFSIZE,
                "\tv%d_%lld [shape = circle, style = filled, "
                "color = \"#81B65D\", fillcolor = \"#F6FAF4\", "
                "penwidth = 2.5, label = \"\"];", i, (llint)j);
            sbddextended_writeLine(ss, fp);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_zbdd != 0) {
                    child = bddgetchildz(node, k);
                } else {
                    child = bddgetchildb(node, k);
                }
                if (!bddisterminal(child)) {
                    clevel = (int)bddgetlev(child);
                    c = sbddextended_MyDict_find(&node_index->node_dict_arr[clevel],
                                                    (llint)child, &cvalue);
                    assert(c != 0);

                    n = sbddextended_snprintf4(ss, sbddextended_BUFSIZE,
                        "\tv%d_%lld -> v%d_%lld", i, (llint)j,
                        clevel, cvalue);
                    n += sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE,
                    " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        n += sbddextended_snprintf0(ss + n,
                            sbddextended_BUFSIZE, ", style = dotted");
                    }
                    sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE,
                        "];");
                    sbddextended_writeLine(ss, fp);
                } else {
                    n = sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                        "\tv%d_%lld -> t%d", i, (llint)j,
                        (child == bddfalse ? 0 : 1));
                    n += sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE,
                        " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        n += sbddextended_snprintf0(ss + n,
                            sbddextended_BUFSIZE, ", style = dotted");
                    }
                    sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE,
                        "];");
                    sbddextended_writeLine(ss, fp);
                }
            }
        }
        n = sbddextended_snprintf1(ss, sbddextended_BUFSIZE,
            "\t{rank = same; r%d; ", i);
        for (j = 0; j < node_index->level_vec_arr[i].count; ++j) {
            n += sbddextended_snprintf2(ss + n, sbddextended_BUFSIZE,
                "v%d_%lld; ", i, (llint)j);
            if (j % 10 == 9 && j < node_index->level_vec_arr[i].count - 1) {
                sbddextended_writeLine(ss, fp);
                n = sbddextended_snprintf0(ss, sbddextended_BUFSIZE, "\t\t");
            }
        }
        n += sbddextended_snprintf0(ss + n, sbddextended_BUFSIZE, "}");
        sbddextended_writeLine(ss, fp);
    }

    sbddextended_writeLine("\t{rank = same; r0; t0; t1; }", fp);
    sbddextended_writeLine("}", fp);

    if (is_making_index) {
        bddNodeIndex_destruct(node_index);
    }
}

#ifdef __cplusplus

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(FILE* fp, const BDD& bdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphviz_inner(fp, bdd.GetID(), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(FILE* fp, const BDD& bdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsGraphviz<int>(fp, bdd, option, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(std::ostream& ost, const BDD& bdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphviz_inner(NULL, bdd.GetID(), bnode_index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsGraphviz(std::ostream& ost, const BDD& bdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsGraphviz<int>(ost, bdd, option, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(FILE* fp, const ZBDD& zbdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddexportbddasgraphviz_inner(fp, zbdd.GetID(), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(FILE* fp, const ZBDD& zbdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsGraphviz<int>(fp, zbdd, option, NULL);
}

template <typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(std::ostream& ost, const ZBDD& zbdd,
                            std::map<std::string, std::string>* /*option*/,
                            DDIndex<T>* node_index)
{
    bddNodeIndex* bnode_index = NULL;
    if (node_index != NULL) {
        bnode_index = node_index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddexportbddasgraphviz_inner(NULL, zbdd.GetID(), bnode_index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsGraphviz(std::ostream& ost, const ZBDD& zbdd,
                            std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsGraphviz<int>(ost, zbdd, option, NULL);
}

sbddextended_INLINE_FUNC
void bddexportbddasgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    /* can be used for BDD/ZBDD */
    bddexportbddasgraphviz_inner(fp, f, node_index, -1, wo);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphviz(FILE* fp, bddp f,
                                bddNodeIndex* node_index)
{
    WriteObject wo(false, true, NULL);
    /* only for ZBDD */
    bddexportbddasgraphviz_inner(fp, f, node_index, 1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddexportbddasgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* node_index)
{
    /* can be used for BDD/ZBDD */
    bddexportbddasgraphviz_inner(fp, f, node_index, -1);
}

sbddextended_INLINE_FUNC
void bddexportzbddasgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* node_index)
{
    /* only for ZBDD */
    bddexportbddasgraphviz_inner(fp, f, node_index, 1);
}

#endif


#ifdef __cplusplus /* currently svg is supported only in C++ version */

struct ExportAsSvg_arcinfo {
public:
    bddp f;
    int arc; /* 0 or 1-arc */
    int fposx;
    int fposy;

    ExportAsSvg_arcinfo(bddp ff, int a, int fpx, int fpy)
        : f(ff), arc(a), fposx(fpx), fposy(fpy) { }
};

sbddextended_INLINE_FUNC
bool ExportAsSvg_arcinfo_compare(const ExportAsSvg_arcinfo& a1,
                                    const ExportAsSvg_arcinfo& a2)
{
    return a1.fposx < a2.fposx;
}

sbddextended_INLINE_FUNC
int ExportAsSvg_getCirclePosX(int x, int r, double rad)
{
    return x + static_cast<int>(r * cos(rad));
}

sbddextended_INLINE_FUNC
int ExportAsSvg_getCirclePosY(int y, int r, double rad)
{
    return y - static_cast<int>(r * sin(rad));
}

template<typename T>
sbddextended_INLINE_FUNC
void bddexportassvg_inner(FILE* fp, bddp f,
                            DDIndex<T>* index, int is_zbdd
/*#ifdef __cplusplus */
                            , const WriteObject& sbddextended_writeLine
/*#endif */
                        )
{
    char ss[sbddextended_BUFSIZE];
    const int node_radius = 20;
    const int node_interval_x = 30;
    const int node_interval_y = 40;
    const int terminal_x = 30;
    const int terminal_y = 40;
    const int margin_x = 20;
    const int margin_y = 20;
    const int label_y = 7;
    const int arc_width = 3;
    bool draw_zero = true;

    bool is_made = false;
    if (index == NULL) {
        index = new DDIndex<T>(f);
        is_made = true;
    }

    std::map<bddp, std::pair<int, int> > pos_map;
    std::map<bddp, std::vector<ExportAsSvg_arcinfo> > dest_info;
    std::map<bddp, std::pair<int, int> > dest0_pos;
    std::map<bddp, std::pair<int, int> > dest1_pos;
    int y = margin_y + node_radius;
    ullint max_nodes = 0;
    for (int level = index->height(); level >= 1; --level) {
        if (max_nodes < index->size(level)) {
            max_nodes = index->size(level);
        }
    }
    if (max_nodes < 2) {
        max_nodes = 2;
    }
    const int node_x = static_cast<int>((2 * node_radius + 1)
        * max_nodes
        + node_interval_x * (max_nodes + 1));

    for (int level = index->height(); level >= 1; --level) {
        int num_nodes = static_cast<int>(index->size(level));
        int x = node_x / (num_nodes + 1) - (node_radius + node_interval_x - margin_x);
        for (llint j = 0; j < num_nodes; ++j) {
            bddp g = index->getNode(level, j).getBddp();
            bddp g0 = (is_zbdd ? bddgetchild0z(g) : bddgetchild0b(g));
            bddp g1 = (is_zbdd ? bddgetchild1z(g) : bddgetchild1b(g));
            pos_map[g] = std::make_pair(x, y);
            dest_info[g0].push_back(ExportAsSvg_arcinfo(g, 0, x, y));
            dest_info[g1].push_back(ExportAsSvg_arcinfo(g, 1, x, y));
            x += node_x / (num_nodes + 1);
        }
        y += 2 * node_radius + node_interval_y;
    }
    y += terminal_y / 2 - node_radius;
    const int max_x = static_cast<int>(2 * node_radius * max_nodes
        + node_interval_x * (max_nodes - 1)
        + 2 * margin_x);
    const int max_y = y + terminal_y / 2 + margin_y;
    const int num_terms = (draw_zero ? 2 : 1);
    int tx = node_x / (num_terms + 1) - (node_radius + node_interval_x - margin_x);
    if (draw_zero) {
        pos_map[bddempty] = std::make_pair(tx, y);
        tx += node_x / (num_terms + 1);
    }
    pos_map[bddsingle] = std::make_pair(tx, y);

    std::map<bddp, std::vector<ExportAsSvg_arcinfo> >::iterator itor = dest_info.begin();

    while (itor != dest_info.end()) {
        bddp g = itor->first;
        std::vector<ExportAsSvg_arcinfo>& infovec = itor->second;
        std::sort(infovec.begin(), infovec.end(),
            ExportAsSvg_arcinfo_compare);
        double rad = 2.0 / 3.0 * M_PI;
        if (infovec.size() == 1) {
            rad = 1.0 / 2.0 * M_PI;
        }
        for (int i = 0; i < static_cast<int>(infovec.size()); ++i) {
            int posx = ExportAsSvg_getCirclePosX(pos_map[g].first, node_radius, rad);
            int posy = ExportAsSvg_getCirclePosY(pos_map[g].second, node_radius, rad);
            if (infovec[i].arc == 0) {
                dest0_pos[infovec[i].f] = std::make_pair(posx, posy);
            } else {
                assert(infovec[i].arc == 1);
                dest1_pos[infovec[i].f] = std::make_pair(posx, posy);
            }
            if (infovec.size() >= 2) {
                rad -= M_PI / 3.0 / static_cast<double>(infovec.size() - 1);
            }
        }
        ++itor;
    }

    /* draw svg */
    sbddextended_snprintf2(ss, sbddextended_BUFSIZE,
        "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        "width=\"%d\" height=\"%d\">", max_x, max_y);
    sbddextended_writeLine(ss, fp);

    sbddextended_writeLine("<marker id=\"arrow\" viewBox=\"-10 -4 20 8\" "
        "markerWidth=\"10\" markerHeight=\"10\" orient=\"auto\">", fp);
    sbddextended_writeLine("    <polygon points=\"-10,-4 0,0 -10,4\" "
        "fill=\"#1b3966\" stroke=\"none\" />", fp);
    sbddextended_writeLine("</marker>", fp);

    /* draw nodes */
    for (int level = index->height(); level >= 1; --level) {
        for (ullint j = 0; j < index->size(level); ++j) {
            bddp g = index->getNode(level, j).getBddp();
            sbddextended_snprintf4(ss, sbddextended_BUFSIZE,
                "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"#deebf7\" "
                "stroke=\"#1b3966\" stroke-width=\"%d\" />",
                pos_map[g].first, pos_map[g].second, node_radius, arc_width);
            sbddextended_writeLine(ss, fp);
            sbddextended_snprintf3(ss, sbddextended_BUFSIZE,
                "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" "
                "font-size=\"24\">%d</text>", pos_map[g].first,
                pos_map[g].second + label_y, bddvaroflev(level));
            sbddextended_writeLine(ss, fp);
        }
    }

    /* draw arcs */
    for (int level = index->height(); level >= 1; --level) {
        for (ullint j = 0; j < index->size(level); ++j) {
            bddp g = index->getNode(level, j).getBddp();
            int posx1 = ExportAsSvg_getCirclePosX(pos_map[g].first,
                node_radius, 4.0 / 3.0 * M_PI);
            int posy1 = ExportAsSvg_getCirclePosY(pos_map[g].second,
                node_radius, 4.0 / 3.0 * M_PI);
            int posx2 = dest0_pos[g].first;
            int posy2 = dest0_pos[g].second;
            sbddextended_snprintf5(ss, sbddextended_BUFSIZE,
                "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
                "stroke=\"#1b3966\" stroke-width=\"%d\" "
                "stroke-dasharray=\"10,5\" "
                "marker-end=\"url(#arrow)\" />",
                posx1, posy1, posx2, posy2, arc_width);
            sbddextended_writeLine(ss, fp);
            posx1 = ExportAsSvg_getCirclePosX(pos_map[g].first,
                node_radius, 5.0 / 3.0 * M_PI);
            posy1 = ExportAsSvg_getCirclePosY(pos_map[g].second,
                node_radius, 5.0 / 3.0 * M_PI);
            posx2 = dest1_pos[g].first;
            posy2 = dest1_pos[g].second;
            sbddextended_snprintf5(ss, sbddextended_BUFSIZE,
                "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
                "stroke=\"#1b3966\" stroke-width=\"%d\" "
                "marker-end=\"url(#arrow)\" />",
                posx1, posy1, posx2, posy2, arc_width);
            sbddextended_writeLine(ss, fp);
        }
    }

    /*draw terminals */
    sbddextended_snprintf5(ss, sbddextended_BUFSIZE,
        "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
        "fill=\"#deebf7\" stroke=\"#1b3966\" stroke-width=\"%d\" />",
        pos_map[bddempty].first - terminal_x / 2,
        pos_map[bddempty].second - terminal_y / 2,
        terminal_x, terminal_y, arc_width);
    sbddextended_writeLine(ss, fp);
    sbddextended_snprintf5(ss, sbddextended_BUFSIZE,
        "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
        "fill=\"#deebf7\" stroke=\"#1b3966\" stroke-width=\"%d\" />",
        pos_map[bddsingle].first - terminal_x / 2,
        pos_map[bddsingle].second - terminal_y / 2,
        terminal_x, terminal_y, arc_width);
    sbddextended_writeLine(ss, fp);

    sbddextended_snprintf2(ss, sbddextended_BUFSIZE,
        "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" "
        "font-size=\"24\">0</text>", pos_map[bddempty].first,
        pos_map[bddempty].second + label_y);
    sbddextended_writeLine(ss, fp);

    sbddextended_snprintf2(ss, sbddextended_BUFSIZE,
        "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" "
        "font-size=\"24\">1</text>", pos_map[bddsingle].first,
        pos_map[bddsingle].second + label_y);
    sbddextended_writeLine(ss, fp);

    sbddextended_writeLine("</svg>", fp);

    if (is_made) {
        delete index;
        index = NULL;
    }
}

template<typename T>
sbddextended_INLINE_FUNC
void exportBDDAsSvg(FILE* fp, const BDD& bdd,
                    std::map<std::string, std::string>* /*option*/,
                    DDIndex<T>* index)
{
    WriteObject wo(false, true, NULL);
    bddexportassvg_inner(fp, bdd.GetID(), index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsSvg(FILE* fp, const BDD& bdd,
                    std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsSvg<int>(fp, bdd, option, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportBDDAsSvg(std::ostream& ost, const BDD& bdd,
                    std::map<std::string, std::string>* /*option*/,
                    DDIndex<T>* index)
{
    WriteObject wo(true, true, &ost);
    bddexportassvg_inner(NULL, bdd.GetID(), index, 0, wo);
}

sbddextended_INLINE_FUNC
void exportBDDAsSvg(std::ostream& ost, const BDD& bdd,
                    std::map<std::string, std::string>* option = NULL)
{
    exportBDDAsSvg<int>(ost, bdd, option, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsSvg(FILE* fp, const ZBDD& zbdd,
                        std::map<std::string, std::string>* /*option*/,
                        DDIndex<T>* index)
{
    WriteObject wo(false, true, NULL);
    bddexportassvg_inner(fp, zbdd.GetID(), index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsSvg(FILE* fp, const ZBDD& zbdd,
                        std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsSvg<int>(fp, zbdd, option, NULL);
}

template<typename T>
sbddextended_INLINE_FUNC
void exportZBDDAsSvg(std::ostream& ost, const ZBDD& zbdd,
                        std::map<std::string, std::string>* /*option*/,
                        DDIndex<T>* index)
{
    WriteObject wo(true, true, &ost);
    bddexportassvg_inner(NULL, zbdd.GetID(), index, 1, wo);
}

sbddextended_INLINE_FUNC
void exportZBDDAsSvg(std::ostream& ost, const ZBDD& zbdd,
                        std::map<std::string, std::string>* option = NULL)
{
    exportZBDDAsSvg<int>(ost, zbdd, option, NULL);
}

#endif


#ifdef __cplusplus
} /* end of namespace sbddh */
#endif

#endif /* SBDD_HELPER_H */

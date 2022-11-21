//
// One header library for SAPPOROBDD C/C++ version
// version 0.05 alpha
//
// Copyright (c) 2017 -- 2022 Jun Kawahara
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
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

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


#ifdef __cplusplus

// need to free the returned value
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
    free(op_stack);
    free(node_stack);
    assert(debug_count == d->count);
    d->count = 0;
    d->root = NULL;
}


sbddextended_INLINE_FUNC
void sbddextended_MyDict_add(sbddextended_MyDict* d, llint key, llint value)
{
    sbddextended_MyDictNode* node;
    if (d->root == NULL) {
        d->root = sbddextended_MyDictNode_makeNewNode(key, value);
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
                    node->left = sbddextended_MyDictNode_makeNewNode(key,
                                                                     value);
                    ++d->count;
                    break;
                }
            } else { // key > node->key
                if (node->right != NULL) {
                    node = node->right;
                } else {
                    node->right = sbddextended_MyDictNode_makeNewNode(key,
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

    dest->root = sbddextended_MyDictNode_makeNewNode(src->root->key,
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
            dest_node_stack[sp] = sbddextended_MyDictNode_makeNewNode(child->key,
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
int sbddextended_readChar_inner(FILE* fp)
{
    return fgetc(fp);
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

sbddextended_INLINE_FUNC
int sbddextended_readUint8_inner(unsigned char* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned char), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint16_inner(unsigned short* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned short), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint32_inner(unsigned int* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned int), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint64_inner(ullint* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(ullint), 1, fp) != 0;
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
        return -1; // never come here
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
            if (str.length() >= sbddextended_MAX_LINE - 1) {
                fprintf(stderr, "Each line must not exceed %d characters\n",
                    sbddextended_MAX_LINE);
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
                    if (stpos_ - start >= sbddextended_MAX_LINE - 1) {
                        fprintf(stderr, "Each line must not exceed %d characters\n",
                                sbddextended_MAX_LINE);
                        exit(1);
                    }
                }
                strncpy(buf, st_ + start, stpos_ - start);
                ++stpos_;
                return true;
            }
        }
        return false; // never come here
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
    return fwrite(&v, sizeof(unsigned char), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint16_inner(unsigned short v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned short), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint32_inner(unsigned int v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned int), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint64_inner(ullint v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(ullint), 1, fp) != 0;
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
        //std::cerr << "uint8 " << (ullint)v << std::endl;
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
        //std::cerr << "uint16 " << (ullint)v << std::endl;
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
        //std::cerr << "uint32 " << (ullint)v << std::endl;
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
        //std::cerr << "uint64 " << (ullint)v << std::endl;
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
        for (j = 0; j < i; ++j) { // check duplicate
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
    int i, c;
    bddvar* ar;

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


// *************************** C++ version start *****************************

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

sbddextended_INLINE_FUNC BDD takeNot(const BDD& f)
{
    return BDD_ID(bddtakenot(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD takeNot(const ZBDD& f)
{
    return ZBDD_ID(bddtakenot(f.GetID()));
}

sbddextended_INLINE_FUNC BDD addNot(const BDD& f)
{
    return BDD_ID(bddaddnot(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD addNot(const ZBDD& f)
{
    return ZBDD_ID(bddaddnot(f.GetID()));
}

sbddextended_INLINE_FUNC BDD eraseNot(const BDD& f)
{
    return BDD_ID(bdderasenot(f.GetID()));
}

sbddextended_INLINE_FUNC ZBDD eraseNot(const ZBDD& f)
{
    return ZBDD_ID(bdderasenot(f.GetID()));
}

sbddextended_INLINE_FUNC bool is64BitVersion()
{
    return bddis64bitversion() != 0;
}

sbddextended_INLINE_FUNC bool isValid(const BDD& f)
{
    return bddisvalid(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isValid(const ZBDD& f)
{
    return bddisvalid(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isTerminal(const BDD& f)
{
    return bddisterminal(f.GetID()) != 0;
}

sbddextended_INLINE_FUNC bool isTerminal(const ZBDD& f)
{
    return bddisterminal(f.GetID()) != 0;
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
    if (isNegative(f)) {
        return ~getChild0(f);
    } else {
        return getChild0(f);
    }
}

sbddextended_INLINE_FUNC ZBDD getChild0Raw(const ZBDD& f)
{
    if (isNegative(f)) {
        return getChild0(f) + ZBDD(1);
    } else {
        return getChild0(f);
    }
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
    if (isNegative(f)) {
        return ~getChild1(f);
    } else {
        return getChild1(f);
    }
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
        return getChild1(f);
    } else {
        return getChild0(f);
    }
}

sbddextended_INLINE_FUNC ZBDD getChildRaw(const ZBDD& f, int child)
{
    if (child == 1) {
        return getChild1(f);
    } else {
        return getChild0(f);
    }
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
    // use set to remove duplicated elements
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

// need to delete[] the returned pointer
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
sbddextended_INLINE_FUNC ZBDD getAllSetsIncluding_inner(const T1& base_variables,
                                                        const T2& target_variables)
{
    ZBDD f = getPowerSet(base_variables);

    bddvar c = 0;
    for (typename T2::const_iterator itor = target_variables.begin();
         itor != target_variables.end(); ++itor) {
        c = *itor;
        if (std::find(base_variables.begin(), base_variables.end(), c) !=
            base_variables.end()) { // c found
            f = f.OnSet(c);
        } else {
            f = f.Change(c);
        }
    }
    return f;
}


template<typename T>
sbddextended_INLINE_FUNC ZBDD getAllSetsIncluding(const T& base_variables,
                                                  const std::vector<bddvar>& target_variables)
{
    return getAllSetsIncluding_inner(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getAllSetsIncluding(const T& base_variables,
                                                  const std::set<bddvar>& target_variables)
{
    return getAllSetsIncluding_inner(base_variables, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getAllSetsIncluding(const T& base_variables,
                                                  int v)
{
    std::vector<bddvar> target_variables;
    target_variables.push_back(v);

    return getAllSetsIncluding(base_variables, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsIncluding(int n,
                                                       const std::vector<bddvar>& target_variables)
{
    std::vector<bddvar> base_variables;
    for (int v = 1; v <= n; ++v) {
        base_variables.push_back(v);
    }

    return getAllSetsIncluding(base_variables, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsIncluding(int n,
                                                       const std::set<bddvar>& target_variables)
{
    std::vector<bddvar> base_variables;
    for (int v = 1; v <= n; ++v) {
        base_variables.push_back(v);
    }

    return getAllSetsIncluding(base_variables, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsIncluding(int n, int v)
{
    std::vector<bddvar> target_variables;
    target_variables.push_back(v);

    return getAllPowerSetsIncluding(n, target_variables);
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getAllPowerSetsNotIncluding_inner(int n,
                                                                const T& target_variables)
{
    ZBDD f = getPowerSet(n);

    for (typename T::const_iterator itor = target_variables.begin();
         itor != target_variables.end(); ++itor) {
        f = f.OffSet(*itor);
    }
    return f;
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsNotIncluding(int n,
                                                          const std::vector<bddvar>& target_variables)
{
    return getAllPowerSetsNotIncluding_inner(n, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsNotIncluding(int n,
                                                          const std::set<bddvar>& target_variables)
{
    return getAllPowerSetsNotIncluding_inner(n, target_variables);
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsNotIncluding(int n, int v)
{
    ZBDD f = getPowerSet(n);
    f = f.OffSet(v);
    return f;
}

template<typename T>
sbddextended_INLINE_FUNC ZBDD getAllSetsWithCard(const T& variables, int k)
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
    std::vector<ZBDD> next(k + 1);

    for (int i = 0; i < k; ++i) {
        current.push_back(ZBDD(0));
    }
    current.push_back(ZBDD(1));

    for (int i = 0; i < n; ++i) {
        int v = bddvaroflev(ar[i]);
        for (int j = 0; j <= std::min(n - i - 1, k); ++j) {
            if (j < k) {
                next[j] = current[j] + current[j + 1].Change(v);
            } else {
                next[j] = current[j];
            }
        }
        current = next;
    }
    delete[] ar;
    return current[0];
}

sbddextended_INLINE_FUNC ZBDD getAllPowerSetsWithCard(int n, int k)
{
    std::vector<bddvar> variables;
    for (int v = 1; v <= n; ++v) {
        variables.push_back(v);
    }
    return getAllSetsWithCard(variables, k);
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

    // translate varIDs to levels
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

#endif

typedef struct tagbddNodeIndex {
    int is_raw;
    // All of the following four pointers are NULL if f is a terminal or bddnull.
    sbddextended_MyDict* node_dict_arr;
    sbddextended_MyVector* level_vec_arr; // stores all nodes at level i
    llint* offset_arr;
    llint* count_arr; // array representing the number of solutions for node i
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

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        index->node_dict_arr = NULL;
        index->level_vec_arr = NULL;
        index->offset_arr = NULL;
        index->count_arr = NULL;
        return index;
    }

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

    if (f == bddnull || f == bddfalse || f == bddtrue) {
        return index;
    }

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
    if (index->f == bddnull || index->f == bddfalse || index->f == bddtrue) {
        return 0ll;
    }
    return (llint)index->offset_arr[0] - sbddextended_BDDNODE_START;
}

sbddextended_INLINE_FUNC
llint bddNodeIndex_sizeAtLevel(const bddNodeIndex* index, int level)
{
    if (index->f == bddnull || index->f == bddfalse || index->f == bddtrue) {
        return 0ll;
    } else if (level <= 0 || index->height < level) {
        return 0ll;
    }
    return (llint)(index->offset_arr[level - 1] - index->offset_arr[level]);
}

sbddextended_INLINE_FUNC
void bddNodeIndex_sizeEachLevel(const bddNodeIndex* index, bddvar* arr)
{
    int i;
    if (!(index->f == bddnull || index->f == bddfalse || index->f == bddtrue)) {
        for (i = 1; i <= index->height; ++i) {
            arr[i] = (bddvar)(index->offset_arr[i - 1] - index->offset_arr[i]);
        }
    }
}

sbddextended_INLINE_FUNC
llint bddNodeIndex_count(const bddNodeIndex* index)
{
    if (index->f == bddnull || index->f == bddfalse) {
        return 0ll;
    } else if (index->f == bddtrue) {
        return 1ll;
    }
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

    if (index != NULL) {
        if (index->level_vec_arr != NULL) {
            for (i = 1; i <= index->height; ++i) {
                sbddextended_MyVector_deinitialize(&index->level_vec_arr[i]);
            }
            free(index->level_vec_arr);
        }
        if (index->node_dict_arr != NULL) {
            for (i = 1; i <= index->height; ++i) {
                sbddextended_MyDict_deinitialize(&index->node_dict_arr[i]);
            }
            free(index->node_dict_arr);
        }
        if (index->offset_arr != NULL) {
            free(index->offset_arr);
        }
        if (index->count_arr != NULL) {
            free(index->count_arr);
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

    bddNodeIndex* getRawPointer()
    {
        return index_;
    }

    llint size()
    {
        return bddNodeIndex_size(index_);
    }

    llint sizeAtLevel(int level)
    {
        return bddNodeIndex_sizeAtLevel(index_, level);
    }

    void sizeEachLevel(std::vector<bddvar>& arr)
    {
        if (!(index_->f == bddnull || index_->f == bddfalse || index_->f == bddtrue)) {
            arr.resize(index_->height + 1);
            for (int i = 1; i <= index_->height; ++i) {
                arr[i] = (bddvar)(index_->offset_arr[i - 1] - index_->offset_arr[i]);
            }
        }
    }

    llint count()
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

class ElementIterator
    : public std::iterator<std::input_iterator_tag, std::set<bddvar> > {
private:
    int sp_;
    std::vector<bddp> bddnode_stack_;
    std::vector<char> op_stack_;
    mutable std::set<bddvar> buffSet_;

    // This function is written by copy-paste of void
    // bddElementIterator_proceed(bddElementIterator* itor).
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

// num_of_variables: 0 -> elements format, non 0 -> value list format
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
    char* value_list;
    char op;
    int i, height, sp, is_first_delim1, is_first_delim2;
    bddvar v;
    bddp node, child;
    char buf[sbddextended_MAX_LINE];

    if (f == bddnull) {
        sbddextended_write("N", fp);
        return;
    } else if (f == bddempty) {
        sbddextended_write("E", fp);
        return;
    } else if (f == bddsingle) {
        if (num_of_variables != 0) {
            for (i = 1; i <= num_of_variables; ++i) {
                sprintf(buf, "0");
                sbddextended_write(buf, fp);
                if (i < num_of_variables) {
                    sprintf(buf, "%s", delim2);
                    sbddextended_write(buf, fp);
                }
            }
        }
        return;
    }

    is_first_delim1 = 1;
    if (bddisnegative(f)) {
        // Output bddsingle first
        if (num_of_variables != 0) {
            for (i = 1; i <= num_of_variables; ++i) {
                sprintf(buf, "0");
                sbddextended_write(buf, fp);
                if (i < num_of_variables) {
                    sprintf(buf, "%s", delim2);
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
                sprintf(buf, "%s", delim1);
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
                    sprintf(buf, "%d", value_list[i]);
                    sbddextended_write(buf, fp);
                    if (i < num_of_variables) {
                        sprintf(buf, "%s", delim2);
                        sbddextended_write(buf, fp);
                    }
                }
            } else {
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
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElements(FILE* fp, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(false, false, NULL);
    const char** arr = sbddextended_strVectorToArray(var_name_map);
    bddprintzbddelements_inner(fp, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, 0, wo);
    free(arr);
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2)
{
    WriteObject wo(true, false, &ost);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void printZBDDElements(std::ostream& ost, const ZBDD& zbdd, const std::string& delim1, const std::string& delim2, const std::vector<std::string>& var_name_map)
{
    WriteObject wo(true, false, &ost);
    const char** arr = sbddextended_strVectorToArray(var_name_map);
    bddprintzbddelements_inner(NULL, zbdd.GetID(), delim1.c_str(), delim2.c_str(), arr, 0, wo);
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
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, 0, wo);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
{
    WriteObject wo(false, false, NULL);
    bddprintzbddelements_inner(fp, f, delim1, delim2, var_name_map, 0, wo);
}

#else

sbddextended_INLINE_FUNC
void bddprintzbddelements(FILE* fp, bddp f, const char* delim1, const char* delim2)
{
    bddprintzbddelements_inner(fp, f, delim1, delim2, NULL, 0);
}

sbddextended_INLINE_FUNC
void bddprintzbddelementswithmap(FILE* fp, bddp f, const char* delim1, const char* delim2,
                                 const char* var_name_map[])
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
    if (zbdd == ZBDD(-1)) { // null
        return std::string("N");
    } if (zbdd == ZBDD(0)) { // 0-terminal
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

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth_inner(FILE* fp, int is_hex, int root_level,
                                        int is_zdd
#ifdef __cplusplus
                             , ReadLineObject& sbddextended_readLine
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
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
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
BDD constructBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
BDD constructBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 0, glo);
    return BDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
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
ZBDD constructZBDDFromFileKnuth(FILE* fp, bool is_hex, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructbddfromfileknuth_inner(fp, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromFileKnuth(std::istream& ist, bool is_hex, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfromfileknuth_inner(NULL, (is_hex ? 1 : 0),
                                           root_level, 1, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromfileknuth(FILE* fp, int is_hex, int root_level = -1)
{
    ReadLineObject glo;
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
bddp bddconstructbddfromgraphillion_inner(FILE* fp, int root_level, int is_zdd
#ifdef __cplusplus
                             , ReadLineObject& sbddextended_readLine
#endif
                             )
{
    int c, level = 0;
    llint i, id, lo, hi, line_count = 0;
    llint max_node_id = 0, root_node_id = 0;
    bddvar var;
    char buf[sbddextended_MAX_LINE];
    char buf1[sbddextended_MAX_LINE];
    char buf2[sbddextended_MAX_LINE];
    char buf3[sbddextended_MAX_LINE];
    char buf4[sbddextended_MAX_LINE];
    bddp p, p0, p1, pf, pfn;
    bddp* bddnode_buf;
    sbddextended_MyVector node_vec, level_vec, lo_vec, hi_vec;

    sbddextended_MyVector_initialize(&node_vec);
    sbddextended_MyVector_initialize(&level_vec);
    sbddextended_MyVector_initialize(&lo_vec);
    sbddextended_MyVector_initialize(&hi_vec);

    while (sbddextended_readLine(buf, fp)) {
        ++line_count;
        if (buf[0] == '.') { // end of file
            break;
        }
        // # Fix ME!  buffer over run
        c = sscanf(buf, "%s %s %s %s", buf1, buf2, buf3, buf4);
        if (c < 4) {
            fprintf(stderr, "Format error in line %lld\n", line_count);
            exit(1);
        }
        if (buf3[0] == 'B') {
            lo = 0;
        } else if (buf3[0] == 'T') {
            lo = 1;
        } else { // # Fix ME! atoi -> atoll ?
            lo = atoi(buf3) + 2;
        }
        if (buf4[0] == 'B') {
            hi = 0;
        } else if (buf4[0] == 'T') {
            hi = 1;
        } else { // # Fix ME! atoi -> atoll ?
            hi = atoi(buf4) + 2;
        }
        sbddextended_MyVector_add(&node_vec, (llint)atoi(buf1) + 2);
        sbddextended_MyVector_add(&level_vec, (llint)atoi(buf2));
        sbddextended_MyVector_add(&lo_vec, (llint)lo);
        sbddextended_MyVector_add(&hi_vec, (llint)hi);
        if (max_node_id < (llint)atoi(buf1) + 2) {
            max_node_id = (llint)atoi(buf1) + 2;
        }
        if (level < (llint)atoi(buf2)) {
            level = (llint)atoi(buf2);
        }
    }

    if (root_level < 0) {
        root_level = level;
    } else if (root_level < level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "larger than the height of the ZBDD.\n");
        exit(1);
    }

    while ((int)bddvarused() < level) {
        bddnewvar();
    }

    bddnode_buf = (bddp*)malloc((ullint)(max_node_id + 1) * sizeof(bddp));
    bddnode_buf[0] = (is_zdd == 0 ? bddfalse : bddempty);
    bddnode_buf[1] = (is_zdd == 0 ? bddtrue : bddsingle);

    for (i = 0; i < (llint)node_vec.count; ++i) {
        id = sbddextended_MyVector_get(&node_vec, i);
        level = (int)sbddextended_MyVector_get(&level_vec, i);
        lo = sbddextended_MyVector_get(&lo_vec, i);
        hi = sbddextended_MyVector_get(&hi_vec, i);
        var = bddvaroflev((bddvar)(root_level - level + 1));
        if (is_zdd == 0) { // BDD
            pf = bddprime(var);
            pfn = bddnot(pf);
            p0 = bddand(bddnode_buf[lo], pfn);
            p1 = bddand(bddnode_buf[hi], pf);
            bddnode_buf[id] = bddor(p0, p1);
            bddfree(pf);
            bddfree(pfn);
            bddfree(p0);
            bddfree(p1);
        } else { // ZDD
            p0 = bddnode_buf[lo];
            p1 = bddchange(bddnode_buf[hi], var);
            bddnode_buf[id] = bddunion(p0, p1);
            bddfree(p1);
        }
        if (level == 1) {
            root_node_id = id;
        }
    }
    // # FIX ME: need bddfree
    //for (i = (llint)lo_vec.count - 1;
    //     i >= sbddextended_BDDNODE_START + 1; --i) {
    //    bddfree(bddnode_buf[i]);
    //}

    p = bddnode_buf[root_node_id];

    free(bddnode_buf);

    sbddextended_MyVector_deinitialize(&hi_vec);
    sbddextended_MyVector_deinitialize(&lo_vec);
    sbddextended_MyVector_deinitialize(&level_vec);
    sbddextended_MyVector_deinitialize(&node_vec);

    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromGraphillion(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfromgraphillion_inner(NULL, root_level, 1, glo);
    return ZBDD_ID(p);
}

#endif


sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements_inner_getoneset(const char* line, int line_len,
                                                  const char* small_sep,
                                                  int small_sep_len)
{
    char int_buf[sbddextended_MAX_LINE];
    int mode = 0;
    int pos;
    int v;
    int sep_pos = 0;
    int sep_start = 0;
    int num_start = 0;
    bddvar vararr[256];
    int vararr_size = 0;

    if (line_len == 0) {
        return bddsingle;
    }

    for (pos = 0; pos < line_len + 1; ++pos) {
        if (pos < line_len) {
            if (mode == 0) {
                if (line[pos] == small_sep[0]) {
                    mode = 1;
                    sep_start = pos;
                    sep_pos = 1;
                }
            } else if (mode == 1) {
                if (line[pos] == small_sep[sep_pos]) {
                    ++sep_pos;
                } else {
                    mode = 0;
                }
            }
        }
        if (pos >= line_len || (mode == 1 && sep_pos >= small_sep_len)) {
            if (pos >= line_len) {
                sep_start = pos;
            }
            assert(num_start < sep_start);
            strncpy(int_buf, line + num_start, (size_t)(sep_start - num_start));
            int_buf[sep_start - num_start] = '\0';
            if (sscanf(int_buf, "%d", &v) != 1) {
                fprintf(stderr, "format error in "
                        "bddconstructzbddfromelements_inner_getoneset\n");
            }
            vararr[vararr_size] = (bddvar)v;
            ++vararr_size;
            num_start = pos + 1;
        }
    }

    //printf("vararr_size = %d\n", vararr_size);
    //for (v = 0; v < vararr_size; ++v) {
    //    printf("<%d> ", vararr[v]);
    //}
    //printf("\n");

    return bddgetsingleset(vararr, vararr_size);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements_inner(FILE* fp, const char* large_sep,
                                        const char* small_sep
#ifdef __cplusplus
                             , ReadCharObject& sbddextended_readChar
#endif
                             )
{
    char buf[sbddextended_MAX_LINE];
    bddp p, q, r;
    int pos;
    int start_pos;
    int sep_pos;
    int c;
    int large_sep_len;
    int small_sep_len;
    int mode;

    p = bddfalse;
    large_sep_len = (int)strlen(large_sep);
    if (large_sep_len <= 0) {
        fprintf(stderr, "large_sep_len must be at least one");
        exit(1);
    }
    small_sep_len = (int)strlen(small_sep);
    if (small_sep_len <= 0) {
        fprintf(stderr, "small_sep_len must be at least one");
        exit(1);
    }

    start_pos = 0;
    sep_pos = 0;
    mode = 0;
    pos = 0;
    while (1 == 1) {
        c = sbddextended_readChar(fp);
        if (c != -1) {
            if (mode == 0) {
                if (c == large_sep[0]) {
                    mode = 1;
                    sep_pos = 1;
                } else {
                    if (pos - start_pos >= sbddextended_MAX_LINE) {
                        fprintf(stderr, "length must not exceed sbddextended_MAX_LINE");
                        exit(1);
                    }
                    buf[pos - start_pos] = (char)c;
                    ++pos;
                }
            } else if (mode == 1) {
                if (c == large_sep[sep_pos]) {
                    ++sep_pos;
                } else {
                    if (pos - start_pos + sep_pos >= sbddextended_MAX_LINE) {
                        fprintf(stderr, "length must not exceed sbddextended_MAX_LINE");
                        exit(1);
                    }
                    memcpy(buf + pos - start_pos, large_sep, (size_t)sep_pos);
                    pos += sep_pos;
                    sep_pos = 0;
                }
            }
        }
        if (c == -1 || sep_pos >= large_sep_len) {
            buf[pos - start_pos] = '\0';

            //fprintf(stderr,"<<%s>>\n", buf);
            // obtain buf
            q = bddconstructzbddfromelements_inner_getoneset(buf,
                                                             pos - start_pos,
                                                             small_sep,
                                                             small_sep_len);
            r = bddunion(p, q);
            bddfree(p);
            bddfree(q);
            p = r;
            if (c == -1) {
                break;
            }
            start_pos = pos + sep_pos + 1;
            pos += sep_pos + 1;
            mode = 0;
            sep_pos = 0;
        }
    }
    return p;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(FILE* fp, const char* large_sep,
                              const char* small_sep)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructzbddfromelements_inner(fp, large_sep, small_sep,
                                           glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromElements(std::istream& ist, const char* large_sep,
                              const char* small_sep)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructzbddfromelements_inner(NULL, large_sep, small_sep,
                                           glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp, const char* large_sep,
                                  const char* small_sep)
{
    ReadLineObject glo;
    return bddconstructzbddfromelements_inner(fp, large_sep, small_sep,
                                              glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzbddfromelements(FILE* fp, const char* large_sep,
                                  const char* small_sep)
{
    return bddconstructzbddfromelements_inner(fp, large_sep, small_sep);
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
void writeZBDDToFileKnuth(FILE* fp, const ZBDD& zbdd, bool is_hex)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtofileknuth_inner(fp, zbdd.GetID(), (is_hex ? 1 : 0), wo);
}

sbddextended_INLINE_FUNC
void writeZBDDToFileKnuth(std::ostream& ost, const ZBDD& zbdd, bool is_hex)
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

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion_inner(FILE* fp, bddp f,
                                   bddNodeIndex* index, int is_zbdd
#ifdef __cplusplus
                        , const WriteObject& sbddextended_writeLine
#endif
                             )
{
    int i, k, n;
    size_t j;
    bddp node, child;
    int is_making_index = 0;
    char ss[sbddextended_TEMP_BUFSIZE];

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

    if (index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            index = bddNodeIndex_makeIndexBWithoutCount(f);
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

    for (i = 1; i <= index->height; ++i) {
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            n = sprintf(ss, "%lld %d", (llint)node, i);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_zbdd != 0) {
                    child = bddgetchildz(node, k);
                } else {
                    child = bddgetchildb(node, k);
                }
                if (!bddisterminal(child)) {
                    n += sprintf(ss + n, " %lld", (llint)child);
                } else if (child == bddempty) {
                    n += sprintf(ss + n, " B");
                } else if (child == bddsingle) {
                    n += sprintf(ss + n, " T");
                }
            }
            sbddextended_writeLine(ss, fp);
        }
    }

    sbddextended_writeLine(".", fp);

    if (is_making_index) {
        bddNodeIndex_destruct(index);
    }
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(FILE* fp, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphillion_inner(fp, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphillion(std::ostream& ost, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddwritebddforgraphillion_inner(NULL, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion(FILE* fp, bddp f,
                               bddNodeIndex* index)
{
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphillion_inner(fp, f, index, -1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritebddforgraphillion(FILE* fp, bddp f,
                               bddNodeIndex* index)
{
    bddwritebddforgraphillion_inner(fp, f, index, -1);
}

#endif


sbddextended_INLINE_FUNC
void bddwritebddforgraphviz_inner(FILE* fp, bddp f,
                                   bddNodeIndex* index, int is_zbdd
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
    char ss[sbddextended_TEMP_BUFSIZE];

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

    if (index == NULL && !(f == bddempty || f == bddsingle)) {
        is_making_index = 1;
        if (is_zbdd != 0) {
            index = bddNodeIndex_makeIndexZWithoutCount(f);
        } else {
            index = bddNodeIndex_makeIndexBWithoutCount(f);
        }
    }

    sbddextended_writeLine("digraph {", fp);
    // print terminals
    if (f != bddsingle) {
        sbddextended_writeLine("\tt0 [label = \"0\", shape = box, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, height = 0.6, fontsize = 24];", fp);
    }
    if (f != bddempty) {
        sbddextended_writeLine("\tt1 [label = \"1\", shape = box, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, width = 0.4, height = 0.6, fontsize = 24];", fp);
    }
    if (f == bddempty || f == bddsingle) {
        sbddextended_writeLine("}", fp);
        return;
    }

    // print vars and levels
    sprintf(ss, "\tr%d [shape = plaintext, label = \"var level\"]", index->height + 1);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "\tr%d [shape = plaintext, label = \"%4d%7d\"]", index->height, bddvaroflev((bddvar)index->height), index->height);
    sbddextended_writeLine(ss, fp);
    sprintf(ss, "\tr%d -> r%d [style = invis];", index->height + 1, index->height);
    sbddextended_writeLine(ss, fp);
    for (i = index->height; i >= 1; --i) {
        if (i > 1) {
            sprintf(ss, "\tr%d [shape = plaintext, label = \"%4d%7d\"];", i - 1, bddvaroflev((bddvar)(i - 1)), i - 1);
            sbddextended_writeLine(ss, fp);
        } else {
            sbddextended_writeLine("\tr0 [style = invis];", fp);
        }
        sprintf(ss, "\tr%d -> r%d [style = invis];", i, i - 1);
        sbddextended_writeLine(ss, fp);
    }

    for (i = index->height; i >= 1; --i) {
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            sprintf(ss, "\tv%d_%lld [shape = circle, style = filled, color = \"#81B65D\", fillcolor = \"#F6FAF4\", penwidth = 2.5, label = \"\"];", i, (llint)j);
            sbddextended_writeLine(ss, fp);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                if (is_zbdd != 0) {
                    child = bddgetchildz(node, k);
                } else {
                    child = bddgetchildb(node, k);
                }
                if (!bddisterminal(child)) {
                    clevel = (int)bddgetlev(child);
                    c = sbddextended_MyDict_find(&index->node_dict_arr[clevel],
                                                 (llint)child, &cvalue);
                    assert(c != 0);

                    n = sprintf(ss, "\tv%d_%lld -> v%d_%lld", i, (llint)j,
                                clevel, cvalue);
                    n += sprintf(ss + n, " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        n += sprintf(ss + n, ", style = dotted");
                    }
                    sprintf(ss + n, "];");
                    sbddextended_writeLine(ss, fp);
                } else {
                    n = sprintf(ss, "\tv%d_%lld -> t%d", i, (llint)j,
                            (child == bddfalse ? 0 : 1));
                    n += sprintf(ss + n, " [color = \"#81B65D\", penwidth = 2.5");
                    if (k == 0) {
                        n += sprintf(ss + n, ", style = dotted");
                    }
                    sprintf(ss + n, "];");
                    sbddextended_writeLine(ss, fp);
                }
            }
        }
        n = sprintf(ss, "\t{rank = same; r%d; ", i);
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            n += sprintf(ss + n, "v%d_%lld; ", i, (llint)j);
        }
        n += sprintf(ss + n, "}");
        sbddextended_writeLine(ss, fp);
    }

    sbddextended_writeLine("\t{rank = same; r0; t0; t1; }", fp);
    sbddextended_writeLine("}", fp);

    if (is_making_index) {
        bddNodeIndex_destruct(index);
    }
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(FILE* fp, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphviz_inner(fp, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDForGraphviz(std::ostream& ost, const ZBDD& zbdd, DDNodeIndex* index = NULL)
{
    bddNodeIndex* bindex = NULL;
    if (index != NULL) {
        bindex = index->getRawPointer();
    }
    WriteObject wo(true, true, &ost);
    bddwritebddforgraphviz_inner(NULL, zbdd.GetID(), bindex, 1, wo);
}

sbddextended_INLINE_FUNC
void bddwritebddforgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* index)
{
    WriteObject wo(false, true, NULL);
    bddwritebddforgraphviz_inner(fp, f, index, -1, wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritebddforgraphviz(FILE* fp, bddp f,
                            bddNodeIndex* index)
{
    bddwritebddforgraphviz_inner(fp, f, index, -1);
}

#endif


sbddextended_INLINE_FUNC
bddp bddconstructbddfrombinary_inner(FILE* fp, int root_level
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
    bddp p0, p1, q;
    bddp* bddnode_buf;
    sbddextended_MyVector level_vec;
    unsigned char use_negative_arcs;
    unsigned char v8;
    unsigned short v16;
    ullint v64;

    // read head 'B' 'D' 'D'
    for (i = 0; i < 3; ++i) {
        sbddextended_readUint8(&v8, fp);
        if ((i == 0 && v8 != 'B') || (i >= 1 && v8 != 'D')) {
            fprintf(stderr, "This binary is not in the BDD binary format.\n");
            return bddnull;
        }
    }
    sbddextended_readUint8(&v8, fp); // version
    if (v8 != 1) {
        fprintf(stderr, "This function supports only version 1.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); // type
    if (!(v8 == 1 || v8 == 3)) {
        fprintf(stderr, "Currently, this version supports only ZDD.\n");
        return bddnull;
    }

    sbddextended_readUint16(&v16, fp); // number_of_arcs
    if (v16 != 2) {
        fprintf(stderr, "Currently, this function supports only 2 branches.\n");
        return bddnull;
    }

    sbddextended_readUint32(&number_of_terminals, fp); // number_of_terminals
    if (number_of_terminals != 2) {
        fprintf(stderr, "Currently, this function supports only 2 terminals.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); // number_of_bits_for_level
    if (v8 != 16) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_level == 16.\n");
        return bddnull;
    }

    sbddextended_readUint8(&v8, fp); // number_of_bits_for_id
    if (v8 != 64) {
        fprintf(stderr, "Currently, this function supports only the case of number_of_bits_for_id == 64.\n");
        return bddnull;
    }

    sbddextended_readUint8(&use_negative_arcs, fp); // use_negative_arcs
    sbddextended_readUint64(&max_level, fp); // max_level

    if (root_level < 0) {
        root_level = (int)max_level;
    } else if (root_level < (int)max_level) {
        fprintf(stderr, "The argument \"root_level\" must be "
                "at least the height of the ZBDD.\n");
        return bddnull;
    }

    sbddextended_readUint64(&v64, fp); // number_of_roots
    if (v64 != 1) {
        fprintf(stderr, "Currently, this function supports only 1 root.\n");
        return bddnull;
    }
    // reserved
    for (i = 0; i < 8; ++i) {
        sbddextended_readUint64(&v64, fp);
    }

    if (max_level == 0) { // case of a constant function (0/1-terminal)
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

    // level 0, unused (dummy)
    sbddextended_MyVector_add(&level_vec, 0);

    number_of_nodes = number_of_terminals;
    for (level = 1; level <= max_level; ++level) {
        sbddextended_readUint64(&v64, fp);
        sbddextended_MyVector_add(&level_vec, (llint)v64);
        number_of_nodes += v64;
    }
    sbddextended_readUint64(&root_id, fp);

    bddnode_buf = (bddp*)malloc((number_of_nodes + 1) * sizeof(bddp));
    if (bddnode_buf == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    bddnode_buf[0] = bddempty;
    bddnode_buf[1] = bddsingle;

    node_count = number_of_terminals;
    while (sbddextended_readUint64(&v64, fp)) {
        //fprintf(stderr, "%lld\n", v64);
        if (use_negative_arcs != 0) {
            assert(v64 <= 1 || v64 % 2 == 0);
            v64 >>= 1;
        }
        //fprintf(stderr, "0-child: %lld\n", v64);
        p0 = bddnode_buf[v64];
        if (!sbddextended_readUint64(&v64, fp)) {
            fprintf(stderr, "illegal format\n");
            return bddnull;
        }
        if (use_negative_arcs) {
            q = bddnode_buf[v64 >> 1];
            if (v64 % 2 == 1) {
                q = bddtakenot(q);
            }
        } else {
            q = bddnode_buf[v64];
        }
        //fprintf(stderr, "1-child: %lld\n", v64 >> 1);
        node_sum = number_of_terminals;
        for (level = 1; level <= max_level; ++level) {
            // add the number of nodes at the level
            node_sum += (ullint)sbddextended_MyVector_get(&level_vec, (llint)level);
            //fprintf(stderr, "node_sum: %lld\n", node_sum);
            if (node_sum > node_count) {
                break;
            }
        }
        //fprintf(stderr, "level: %lld\n", level);
        assert(level <= max_level);
        p1 = bddchange(q, (bddvar)((int)level + root_level - (int)max_level));
        bddnode_buf[node_count] = bddunion(p0, p1);
        bddfree(p1);
        ++node_count;
    }
    if (use_negative_arcs) {
        if (root_id % 2 == 1) { // negative arc
            return bddtakenot(bddnode_buf[root_id >> 1]);
        } else {
            return bddnode_buf[root_id >> 1];
        }
    } else {
        return bddnode_buf[root_id];
    }
    return bddnull;
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromBinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    bddp p;
    p = bddconstructbddfrombinary_inner(fp,
                                        root_level, glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
ZBDD constructZBDDFromBinary(std::istream& ist, int root_level = -1)
{
    ReadLineObject glo(&ist);
    bddp p;
    p = bddconstructbddfrombinary_inner(NULL,
                                        root_level, glo, glo, glo, glo);
    return ZBDD_ID(p);
}

sbddextended_INLINE_FUNC
bddp bddconstructzbddfrombinary(FILE* fp, int root_level = -1)
{
    ReadLineObject glo;
    return bddconstructbddfrombinary_inner(fp,
                                           root_level, glo, glo, glo, glo);
}

#else

sbddextended_INLINE_FUNC
bddp bddconstructzbddfrombinary(FILE* fp, int root_level)
{
    return bddconstructbddfrombinary_inner(fp, root_level);
}

#endif



sbddextended_INLINE_FUNC
void bddwritezbddtobinary_inner(FILE* fp, bddp f
#ifdef __cplusplus
                          , const WriteObject& sbddextended_writeUint8
                          , const WriteObject& sbddextended_writeUint16
                          , const WriteObject& sbddextended_writeUint32
                          , const WriteObject& sbddextended_writeUint64
#endif
                             )
{
    // Since only BDD/ZDD is treated in the current version,
    // the number of terminals is fixed to be 2.
    const unsigned int number_of_terminals = 2;
    const unsigned char use_negative_arcs = 1;
    ullint i, j;
    int k;
    ullint max_level;
    bddNodeIndex* index;
    ullint number_of_nodes = 0;
    ullint root_id;
    llint id;
    ullint* id_prefix;
    bddp node, child;

    max_level = (ullint)bddgetlev(f);

    // start header

    sbddextended_writeUint8((unsigned char)'B', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);
    sbddextended_writeUint8((unsigned char)'D', fp);

    sbddextended_writeUint8((unsigned char)1u, fp); // version
    sbddextended_writeUint8((unsigned char)1u, fp); // DD type
    // number_of_arcs
    sbddextended_writeUint16((unsigned short)2u, fp);
    // number_of_terminals
    sbddextended_writeUint32(number_of_terminals, fp);
    // number_of_bits_for_level
    sbddextended_writeUint8((unsigned char)16u, fp);
    // number_of_bits_for_id
    sbddextended_writeUint8((unsigned char)64u, fp);
    // use_negative_arcs
    sbddextended_writeUint8(use_negative_arcs, fp);
    // max_level
    sbddextended_writeUint64(max_level, fp);
    // number_of_roots
    sbddextended_writeUint64((ullint)1u, fp);

    // reserved
    for (i = 0; i < 8; ++i) {
        sbddextended_writeUint64((ullint)0u, fp);
    }

    // end header

    if (max_level == 0) { // case of a constant function (0/1-terminal)
        if (f == bddempty) {
            sbddextended_writeUint64((ullint)0ull, fp);
        } else if (f == bddsingle) {
            sbddextended_writeUint64((ullint)1ull, fp);
        } else {
            assert(0);
        }
        return;
    }

    index = bddNodeIndex_makeRawIndexZWithoutCount(f);

    assert((ullint)index->height == max_level);

    // write the number of nodes in level i and compute the number of nodes
    for (i = 1; i <= max_level; ++i) {
        sbddextended_writeUint64((ullint)index->level_vec_arr[i].count, fp);
        number_of_nodes += (ullint)index->level_vec_arr[i].count;
    }

    id_prefix = (ullint*)malloc((max_level + 1) * sizeof(ullint));
    if (id_prefix == NULL) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    id_prefix[1] = number_of_terminals;
    for (i = 1; i < max_level; ++i) {
        id_prefix[i + 1] = id_prefix[i] + (ullint)index->level_vec_arr[i].count;
    }

    // write the number of the root id
    // (In the current version, assume that the number of roots is 2.)
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
        for (j = 0; j < index->level_vec_arr[i].count; ++j) {
            node = (bddp)sbddextended_MyVector_get(&index->level_vec_arr[i], (llint)j);
            for (k = 0; k < sbddextended_NUMBER_OF_CHILDREN; ++k) {
                child = bddgetchildzraw(node, k);
                if (bddisterminal(child)) {
                    sbddextended_writeUint64((child == bddsingle ? 1llu : 0llu), fp);
                    //fprintf(stderr, "%d-child: %d\n", k, (child == bddsingle ? 1 : 0));
                } else {
                    if (sbddextended_MyDict_find(&index->node_dict_arr[bddgetlev(child)],
                                                 (llint)bdderasenot(child), &id) == 0) {
                        fprintf(stderr, "node not found\n");
                        return;
                    }
                    id += (llint)id_prefix[bddgetlev(child)];
                    if (use_negative_arcs != 0) {
                        id *= 2;
                        if (bddisnegative(child)) {
                            id += 1;
                        }
                    }
                    sbddextended_writeUint64((ullint)id, fp);
                    //fprintf(stderr, "%d-child: %lld\n", k, id);
                }
            }
        }
    }

    bddNodeIndex_destruct(index);
    free(index);
    free(id_prefix);
}

#ifdef __cplusplus

sbddextended_INLINE_FUNC
void writeZBDDToBinary(FILE* fp, const ZBDD& zbdd)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtobinary_inner(fp, zbdd.GetID(), wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void writeZBDDToBinary(std::ostream& ost, const ZBDD& zbdd)
{
    WriteObject wo(true, true, &ost);
    bddwritezbddtobinary_inner(NULL, zbdd.GetID(), wo, wo, wo, wo);
}

sbddextended_INLINE_FUNC
void bddwritezbddtobinary(FILE* fp, bddp f)
{
    WriteObject wo(false, true, NULL);
    bddwritezbddtobinary_inner(fp, f, wo, wo, wo, wo);
}

#else

sbddextended_INLINE_FUNC
void bddwritezbddtobinary(FILE* fp, bddp f)
{
    bddwritezbddtobinary_inner(fp, f);
}

#endif


#ifdef __cplusplus
} // end of namespace sbddh
#endif

#endif // SBDD_HELPER_H

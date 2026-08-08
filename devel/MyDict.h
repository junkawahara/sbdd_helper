#ifndef __cplusplus

/* The dictionary is an AVL tree. Without the rebalancing, inserting the
   keys in the increasing order, which is what the importers of the file
   formats do, would build a tree of height n and make each insertion and
   each lookup take O(n) time. */

typedef struct tagsbddextended_MyDictNode {
    struct tagsbddextended_MyDictNode* left;
    struct tagsbddextended_MyDictNode* right;
    llint key;
    llint value;
    /* the height of the subtree whose root is this node (a leaf has 1) */
    int height;
} sbddextended_MyDictNode;

/* An AVL tree of height h has at least Fib(h + 2) - 1 nodes, so a tree of
   height 92 has more than 2^63 nodes and cannot be built in practice. */
#define sbddextended_MYDICT_MAXHEIGHT 92

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
    node->height = 1;
    return node;
}

sbddextended_INLINE_FUNC
int sbddextended_MyDictNode_height(const sbddextended_MyDictNode* node)
{
    return (node == NULL ? 0 : node->height);
}

sbddextended_INLINE_FUNC
void sbddextended_MyDictNode_updateHeight(sbddextended_MyDictNode* node)
{
    int hl;
    int hr;

    hl = sbddextended_MyDictNode_height(node->left);
    hr = sbddextended_MyDictNode_height(node->right);
    node->height = (hl > hr ? hl : hr) + 1;
}

sbddextended_INLINE_FUNC
sbddextended_MyDictNode* sbddextended_MyDictNode_rotateLeft(
                                        sbddextended_MyDictNode* node)
{
    sbddextended_MyDictNode* r;

    r = node->right;
    node->right = r->left;
    r->left = node;
    sbddextended_MyDictNode_updateHeight(node);
    sbddextended_MyDictNode_updateHeight(r);
    return r;
}

sbddextended_INLINE_FUNC
sbddextended_MyDictNode* sbddextended_MyDictNode_rotateRight(
                                        sbddextended_MyDictNode* node)
{
    sbddextended_MyDictNode* l;

    l = node->left;
    node->left = l->right;
    l->right = node;
    sbddextended_MyDictNode_updateHeight(node);
    sbddextended_MyDictNode_updateHeight(l);
    return l;
}

/* Rebalance the subtree whose root is "node". Both children must be
   balanced and their heights must differ by at most 2. The root of the
   rebalanced subtree is returned. */
sbddextended_INLINE_FUNC
sbddextended_MyDictNode* sbddextended_MyDictNode_balance(
                                        sbddextended_MyDictNode* node)
{
    int balance;

    sbddextended_MyDictNode_updateHeight(node);
    balance = sbddextended_MyDictNode_height(node->left)
                - sbddextended_MyDictNode_height(node->right);
    if (balance > 1) {
        if (sbddextended_MyDictNode_height(node->left->left)
                < sbddextended_MyDictNode_height(node->left->right)) {
            node->left = sbddextended_MyDictNode_rotateLeft(node->left);
        }
        return sbddextended_MyDictNode_rotateRight(node);
    } else if (balance < -1) {
        if (sbddextended_MyDictNode_height(node->right->right)
                < sbddextended_MyDictNode_height(node->right->left)) {
            node->right = sbddextended_MyDictNode_rotateRight(node->right);
        }
        return sbddextended_MyDictNode_rotateLeft(node);
    }
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
        exit(1);
    }
    op_stack = (char*)malloc(stack_size * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        free(node_stack);
        exit(1);
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
    sbddextended_MyDictNode* path[sbddextended_MYDICT_MAXHEIGHT];
    char dir[sbddextended_MYDICT_MAXHEIGHT];
    sbddextended_MyDictNode* node;
    int sp;
    int i;

    /* search for the key, remembering the path from the root */
    sp = 0;
    node = d->root;
    while (node != NULL) {
        if (node->key == key) { /* found */
            node->value = value;
            return;
        }
        assert(sp < sbddextended_MYDICT_MAXHEIGHT);
        path[sp] = node;
        if (key < node->key) {
            dir[sp] = 0;
            node = node->left;
        } else { /* key > node->key */
            dir[sp] = 1;
            node = node->right;
        }
        ++sp;
    }

    node = sbddextended_MyDictNode_makeNewNode(key, value);
    ++d->count;

    /* hang the new node and rebalance the path back to the root */
    for (i = sp - 1; i >= 0; --i) {
        if (dir[i] == 0) {
            path[i]->left = node;
        } else {
            path[i]->right = node;
        }
        node = sbddextended_MyDictNode_balance(path[i]);
    }
    d->root = node;
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

/* "dest" must be initialized before calling this function. */
/* The current content of "dest" is discarded. */
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

    /* discard the current content of dest */
    sbddextended_MyDict_deinitialize(dest);

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
        exit(1);
    }
    dest_node_stack = (sbddextended_MyDictNode**)malloc(stack_size * sizeof(sbddextended_MyDictNode*));
    if (dest_node_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        free(node_stack);
        exit(1);
    }
    op_stack = (char*)malloc(stack_size * sizeof(char));
    if (op_stack == NULL) {
        fprintf(stderr, "out of memory\n");
        free(dest_node_stack);
        free(node_stack);
        exit(1);
    }

    dest->root = sbddextended_MyDictNode_makeNewNode(src->root->key,
                                                        src->root->value);
    dest->root->key = src->root->key;
    dest->root->value = src->root->value;
    dest->root->height = src->root->height;

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
            dest_node_stack[sp]->height = child->height;
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
    free(op_stack);
    free(dest_node_stack);
    free(node_stack);
#endif
}

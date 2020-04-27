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

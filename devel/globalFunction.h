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

#ifndef SBDDH_NO_BDDCT

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

#endif /* SBDDH_NO_BDDCT */

#endif

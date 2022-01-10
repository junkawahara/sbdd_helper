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

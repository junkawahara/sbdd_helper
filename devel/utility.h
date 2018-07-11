
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

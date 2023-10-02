
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

// XOR shift
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

#endif


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
value_t sbddh_getZero()
{
    return value_t(0);
}

template<typename value_t>
value_t sbddh_getOne()
{
    return value_t(1);
}

#ifdef USE_GMP
template<typename value_t>
value_t sbddh_getValueFromMpz(const mpz_class& v);

template<>
mpz_class sbddh_getValueFromMpz<mpz_class>(const mpz_class& v)
{
    return v;
}

template<>
llint sbddh_getValueFromMpz<llint>(const mpz_class& v)
{
    return v.get_ui();
}
#else
template<typename value_t>
value_t sbddh_getValueFromMpz(llint v);

template<>
llint sbddh_getValueFromMpz<llint>(llint v)
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

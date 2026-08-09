
#ifdef __cplusplus

/* need to free the returned value */
sbddextended_INLINE_FUNC
const char** sbddextended_strVectorToArray(const std::vector<std::string>& vec)
{
    const char** arr;

    /* malloc(0) may return NULL, which is not an error. Allocate at least
       one element so that NULL always means out of memory. */
    arr = (const char**)malloc((vec.size() > 0 ? vec.size() : 1)
                                * sizeof(const char*));
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
inline
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

/* Signed overflow is undefined behavior in C++, so the weight
   computations in DDIndex check the range before each operation and
   report an error instead of overflowing. */
sbddextended_INLINE_FUNC
llint sbddh_checkedAdd(llint v1, llint v2)
{
    if ((v2 > 0 && v1 > LLONG_MAX - v2)
            || (v2 < 0 && v1 < LLONG_MIN - v2)) {
        std::cerr << "The weight computation causes an overflow of "
                     "long long int." << std::endl;
        exit(1);
    }
    return v1 + v2;
}

/* Multiplication of a weight and a number of sets. */
sbddextended_INLINE_FUNC
llint sbddh_checkedMul(llint v1, ullint v2)
{
    if (v1 == 0 || v2 == 0) {
        return 0;
    }
    if (v1 > 0) {
        if (v2 > static_cast<ullint>(LLONG_MAX) / static_cast<ullint>(v1)) {
            std::cerr << "The weight computation causes an overflow of "
                         "long long int." << std::endl;
            exit(1);
        }
        return v1 * static_cast<llint>(v2);
    } else {
        /* -LLONG_MIN cannot be represented as llint, so compute the
           absolute value of v1 in ullint. */
        const ullint abs_v1 = static_cast<ullint>(-(v1 + 1)) + 1;
        const ullint limit = static_cast<ullint>(LLONG_MAX) + 1;
        if (v2 > limit / abs_v1) {
            std::cerr << "The weight computation causes an overflow of "
                         "long long int." << std::endl;
            exit(1);
        }
        const ullint product = abs_v1 * v2;
        if (product == limit) {
            return LLONG_MIN;
        }
        return -static_cast<llint>(product);
    }
}

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

/* The following two functions are used to append a string to a buffer of */
/* sbddextended_BUFSIZE bytes in which n characters have already been */
/* written. Note that n may be negative or may exceed the buffer size */
/* because snprintf returns the number of characters that would have been */
/* written if the buffer were large enough. The write position and the */
/* remaining size are clamped so that the appended write never goes */
/* outside the buffer. */
sbddextended_INLINE_FUNC
size_t sbddextended_bufPos(int n)
{
    if (n < 0) {
        return 0;
    } else if ((size_t)n >= (size_t)sbddextended_BUFSIZE) {
        return (size_t)sbddextended_BUFSIZE - 1;
    }
    return (size_t)n;
}

sbddextended_INLINE_FUNC
size_t sbddextended_bufRest(int n)
{
    return (size_t)sbddextended_BUFSIZE - sbddextended_bufPos(n);
}

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

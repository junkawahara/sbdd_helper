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

#ifdef __cplusplus

class ReadLineObject {
private:
    bool is_fstream_;
    std::istream* ist_;

public:
    ReadLineObject(bool is_fstream, std::istream* ist)
        : is_fstream_(is_fstream), ist_(ist) {}

    bool operator()(char* buf, FILE* fp) const {
        if (is_fstream_) {
            std::string str;
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
        } else {
            return sbddextended_readLine_inner(buf, fp) != 0;
        }
    }
};

#else

sbddextended_INLINE_FUNC 
int sbddextended_readLine(char* buf, FILE* fp)
{
    return sbddextended_readLine_inner(buf, fp);
}

#endif
